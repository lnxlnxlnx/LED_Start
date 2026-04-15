#ifndef DD97614E_20F9_43D0_9CE3_ECE964A5C246
#define DD97614E_20F9_43D0_9CE3_ECE964A5C246
/* led_breath.c */
#include "led.h"
#include "ul_thread.h"
#include "ul_timer.h"
#include "delay.h"
#include <math.h>
#include <stdint.h>

#define LED_COUNT       8
#define PWM_PERIOD      100          // PWM 周期（100 级亮度）
#define BREATH_CYCLE    2000         // 呼吸周期 (ms)
#define PWM_FREQ        200          // PWM 频率 (Hz)
#define SYS_TICK_MS     1            // 系统tick单位：1ms（需和系统配置一致）

/* 保存定时器句柄，避免被回收 */
static ul_timer_t *g_breath_timer = NULL;
extern LED_Operate led_funcs[];

/* 8个LED相位偏移（360度均分） */
static uint16_t led_phase[LED_COUNT] = {0, 45, 90, 135, 180, 225, 270, 315};

/* 正弦查找表（反向适配共阳LED：0=最亮，100=最暗） */
static uint8_t sin_table[360] = {0};

/* 全局时间计数器（ms） */
static volatile uint32_t g_system_tick = 0;

/* 每个LED当前占空比（0~100） */
static uint8_t led_duty[LED_COUNT] = {0};

/* 实时更新PWM输出（必须高频、精准执行） */
static void update_led_pwm(void)
{
    static uint8_t pwm_counter = 180;
    //pwm_counter = (pwm_counter + 1) % PWM_PERIOD; // 避免溢出
    
    // 共阳LED：pwm_counter < duty → 点亮（duty越大，点亮时间越长=越亮）
    for (int i = 0; i < LED_COUNT; i++)
    {
        led_funcs[i](pwm_counter < led_duty[i] ? 0 : 1);
    }
}

/* 更新LED占空比（低频，基于正弦曲线） */
static void update_led_duty(void)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        // 计算当前相位（强制取模，避免溢出）
        //float angle = fmod((g_system_tick * 360.0f / BREATH_CYCLE) + phase_offset[i], 360.0f);
        led_phase[i] = (uint32_t)(1 + led_phase[i]) % 360;
        //index = index < 0 ? 0 : (index >= 360 ? 359 : index);
        
        led_duty[i] = sin_table[led_phase[i]];
    }
}

/* 定时器回调（1ms触发，核心：更新tick + 实时PWM） */
void timer_callback(void *parameter)
{
    static uint32_t duty_tick = 0;
    g_system_tick++;
    
    // 高频更新PWM（200Hz = 每5ms更新一次）
    if (g_system_tick % (1000/PWM_FREQ) == 0)
    {
        g_system_tick = 0; // 避免溢出
        update_led_pwm();
    }
    
    // 低频更新占空比（20ms一次）
    duty_tick++;
    if (duty_tick >= 20)
    {
        duty_tick = 0;
        update_led_duty();
    }
}

/* 呼吸灯线程（仅兜底，核心逻辑移到定时器回调） */
static void led_breath_thread(void *arg)
{
    // 线程仅做异常监控，核心逻辑在定时器回调中
    while (1)
    {
        // 检查定时器是否存活，挂了就重启
        if (g_breath_timer == NULL || g_breath_timer->stat == ULOS_TIMER_STAT_STOPPED) // 定时器未启动或已停止
        {
            led_loop_control(); // 降级到测试模式
            ul_timer_start(g_breath_timer);
        }
        ul_thread_delay(100); // 低频检查，减少CPU占用
    }
}

/* 启动呼吸灯（最终可运行版） */
void led_breath_start(void)
{
    // 1. 初始化LED硬件（必须）
    LED_Init();

    for (int i = 0; i < 360; i++){
        sin_table[i] = i;
    }
    
    // 2. 销毁旧定时器（避免重复创建）
    if (g_breath_timer != NULL)
    {
        ul_timer_stop(g_breath_timer);
        ul_timer_delete(g_breath_timer);
    }
    
    // 3. 创建1ms周期定时器（核心：驱动tick和PWM）
    g_breath_timer = ul_timer_create(
        "breath_timer",
        timer_callback,    // 回调函数（核心逻辑）
        NULL,              // 参数
        SYS_TICK_MS,       // 超时时间（1ms，和系统tick单位一致）
        ULOS_TIMER_TYPE_PERIODIC // 周期模式
    );
    
    // 4. 启动定时器（必须）
    if (g_breath_timer != NULL)
    {
        ul_timer_start(g_breath_timer);
    }
    else
    {
        // 定时器创建失败，降级到测试模式
        while (1) led_loop_control();
        return;
    }
    
    // 5. 创建呼吸灯监控线程（增大栈大小，提高优先级）
    ul_thread_t *thread = ul_thread_create(
        "led_breath",
        led_breath_thread,
        NULL,
        2048,   // 增大栈大小，避免浮点运算溢出
        2,      // 高优先级，确保监控有效
        1       // 时间片
    );
    
    // 6. 启动线程
    if (thread != NULL)
    {
        ul_thread_startup(thread);
    }
}

#endif /* DD97614E_20F9_43D0_9CE3_ECE964A5C246 */