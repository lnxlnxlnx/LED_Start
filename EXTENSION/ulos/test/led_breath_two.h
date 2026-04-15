#ifndef DF8EF076_CCD2_4DE8_974A_DE190644C6EE
#define DF8EF076_CCD2_4DE8_974A_DE190644C6EE
/* led_breath.c */
#include "led.h"
#include "ul_thread.h"
#include "ul_timer.h" // Ensure this header file defines the complete ul_timer structure
#include "delay.h"
#include <math.h>
#include <stdint.h>

#define LED_COUNT 8
#define PWM_PERIOD 100 // PWM 周期（100 级亮度）
#define BREATH_STEP 1  // 相位每次递增1度
#define SYS_TICK_MS 1  // 系统tick单位：1ms
#define PWM_FREQ 200   // PWM频率(Hz)，5ms更新一次

/* 保存定时器句柄 */
static ul_timer_t *g_breath_timer = NULL;

extern LED_Operate led_funcs[];

// 全局变量
uint8_t pwm_level = 0;                                     // PWM当前等级（0~127，7bit精度）
uint8_t current_led = 0;                                   // 当前渐变的LED编号（0~7）
uint16_t brightness[8] = {0, 14, 28, 42, 56, 70, 84, 98}; // 8个LED的亮度值（0~127）

// 指数函数拟合亮度（非线性映射）
uint8_t brightness_map(uint8_t level)
{
    // 把0~127映射为指数曲线：y = 2^(x/16) - 1，限制最大值127
    if (level == 0)
        return 0;
    float x = (float)level / 16.0f;
    uint8_t y = (uint8_t)(pow(2, x) - 1);
    return y > 127 ? 127 : y;
}

static void update_pwm(uint8_t mode, uint8_t threshold)
{
    static uint8_t pwm_direction = 1; // 亮度递增方向：1=增加，0=减少
    if (mode)
    {
        // 2. 更新 PWM 计数器
        if (pwm_direction)
        {
            pwm_level++;
            if (pwm_level >= 127)
            {
                pwm_level = 127;
                pwm_direction = 0; // 切换到递减
            }
        }
        else
        {
            if (pwm_level > threshold)
            {
                pwm_level--;
            }
            else
            {
                pwm_level = threshold;
                pwm_direction = 1; // 切换到递增
            }
        }
    }
    else
    {
        pwm_level++;
        if (pwm_level >= 128)
        {
            pwm_level = 0;
            // 可选：在此处更新 brightness 数组实现呼吸效果
        }
    }
}

/* 定时器回调（建议 100us~1ms 触发） */
void timer_callback(void *parameter)
{
    // 1. 遍历所有 LED，根据计数器与亮度比较决定状态
    uint8_t flag = 0;
    for (int j = 0; j < 8; j++)
    {
        uint8_t i;
        if (flag){
            i = j;
        }else{
            i = 7 - j;
        }
        if (pwm_level < brightness[i])
        {
            led_funcs[i](0); // 点亮（低电平有效）
        }
        else
        {
            led_funcs[i](1); // 熄灭
        }
        brightness[i] += 1;
        if (brightness[i] >= 100)
        {
            brightness[i] %= 100; // 循环亮度
        }
    }
    //flag = 1 - (flag); // 反转扫描方向，增加动态感
    update_pwm(0, 50); // 更新 PWM 计数器
}

/* 定时器回调（1ms触发） */
// void timer_callback(void *parameter)
// {
//     static uint32_t pwm_tick = 0;
//     static uint32_t phase_tick = 0;

//     // 2. 计算当前LED的亮度（指数映射）
//     brightness[current_led] = brightness_map(pwm_level);
//     // 3. 更新PWM等级（0~127循环）
//     pwm_level++;
//     if (pwm_level > 127)
//     {
//         pwm_level = 0;
//         // 切换到下一个LED
//         current_led++;
//         if (current_led > 7)
//         {
//             current_led = 0;
//         }
//     }
//     // 4. 根据亮度值控制LED电平（PWM模拟：高电平时间=亮度值/127*1ms）
//     for (int i = 0; i < 8; i++)
//     {
//         if (brightness[i] > 0)
//         {
//             brightness[i] += 16;
//             if (brightness[i] >= 127)
//             {
//                 brightness[i] %= 127;
//             }
//             // 低电平点亮，高电平熄灭（共阳极）
//             led_funcs[i](0); // 点亮
//             // 延时模拟PWM占空比（简单实现，更优方案用DMA+PWM输出）
//             delay_ms((brightness[i] + 1) * 1000 / 127);
//             led_funcs[i](1); // 熄灭
//         }
//     }
// }

/* 呼吸灯监控线程 */
static void led_breath_thread(void *arg)
{
    while (1)
    {
        if (g_breath_timer != NULL && ((ul_timer_t *)g_breath_timer)->stat == ULOS_TIMER_STAT_STOPPED)
        {
            ul_timer_start(g_breath_timer);
        }
        ul_thread_delay(100);
    }
}

/* 启动呼吸灯（最终版） */
void led_breath_start(void)
{
    // 1. 初始化LED硬件
    LED_Init();

    // 3. 销毁旧定时器
    if (g_breath_timer != NULL)
    {
        ul_timer_stop(g_breath_timer);
        ul_timer_delete(g_breath_timer);
        g_breath_timer = NULL;
    }

    // 4. 创建1ms周期定时器
    g_breath_timer = ul_timer_create(
        "breath_timer",
        timer_callback,
        NULL,
        SYS_TICK_MS * 10,
        ULOS_TIMER_TYPE_PERIODIC);

    // 5. 启动定时器
    if (g_breath_timer == NULL)
    {
        while (1)
            led_loop_control();
        return;
    }
    ul_timer_start(g_breath_timer);

    // 6. 创建监控线程
    ul_thread_t *thread = ul_thread_create(
        "led_breath",
        led_breath_thread,
        NULL,
        2048,
        2,
        1);
    if (thread != NULL)
    {
        ul_thread_startup(thread);
    }
}

#endif /* DF8EF076_CCD2_4DE8_974A_DE190644C6EE */
