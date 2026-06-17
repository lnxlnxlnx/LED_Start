/**
 * @file    stopwatch.c
 * @brief   多功能秒表系统 — 实现
 *          基于正点原子 NANO STM32F103 开发板
 *
 * 功能:
 *   1. 8位数码管显示 分-秒-厘秒 (00-00-00 ~ 99-59-99)
 *   2. KEY1 启动/暂停, KEY2 暂停清零, KEY_UP 记次冻结
 *   3. DS0/DS1 1Hz 交替闪烁, DS2~DS7 流水灯
 *   4. 串口日志 (115200) 输出状态变化
 *
 * 调试:
 *   控制台输出带 [停止watch] 标签的状态变化和关键事件。
 *   出现异常时检查: 按键电平、数码管段码、定时器配置。
 *
 * 接线 (开发板已有):
 *   - 数码管: 74HC138(PC10/11/12) + 74HC595(PB3/4/5)
 *   - LED: PC0~PC7 (低电平亮)
 *   - KEY1: PC9 (按下=0)    KEY2: PD2 (按下=0)    WK_UP: PA0 (按下=1)
 *   - USART1: PA9(TX) PA10(RX), 115200-8N1
 */

#include "stopwatch.h"
#include "key.h"       // KEY_Scan()
#include "led.h"       // LED0~LED7 宏
#include "smg.h"       // LED_SMG_WriteNum/LED_SMG_WriteSeg/LED_SMG_Clear
#include "delay.h"     // delay_ms()
#include <stdio.h>     // printf(), sprintf()

/* ======================== 调试开关 ======================== */
#define STOPWATCH_DEBUG    1       // 1=输出调试信息, 0=关闭

/* ======================== 常量定义 ======================== */
#define TICK_10MS_MS        10      // 每个厘秒对应 10ms (TIM3 1ms 中断 10 次)
#define CENTISECOND_PER_SEC 100     // 100 厘秒 = 1 秒
#define SECOND_PER_MIN      60      // 60 秒 = 1 分
#define MINUTE_MAX          99      // 最大 99 分

#define LAP_FREEZE_MS       3000    // 记次冻结持续时间 (毫秒)
#define KEY_SCAN_INTERVAL   20      // 主循环按键扫描间隔 (ms)

#define SEG_DASH            0x02    // 横线 "-" 段码 (共阴: 仅 g 段亮)

/* ======================== 全局变量 (模块内) ======================== */
static Stopwatch_State  g_state = SW_STOPPED;       // 当前状态
static Stopwatch_Time   g_time  = {0, 0, 0};        // 当前计时值
static Stopwatch_Time   g_lap   = {0, 0, 0};        // 记次冻结时的暂存值

static u32 g_freeze_timer = 0;      // 冻结倒计时计数器 (ms)
static u8  g_flow_pos     = 0;      // 流水灯当前位置 (0~5 对应 DS2~DS7)
static u16 g_flow_cnt     = 0;      // 流水灯 tick 计数器

/* ======================== 静态函数声明 ======================== */
static void sw_SetState(Stopwatch_State new_state);
static void sw_UpdateTime(void);
static void sw_UpdateDisplay(void);
static void sw_UpdateLEDs(void);
static void sw_OutputLog(Stopwatch_LogAction action, const Stopwatch_Time *t);

/* ================================================================ */
/*                         公开 API                                  */
/* ================================================================ */

/**
 * @brief   秒表初始化
 *          清零状态/时间, 关闭所有 LED, 数码管显示 00-00-00
 */
void Stopwatch_Init(void)
{
    g_state = SW_STOPPED;
    g_time.minute  = 0;
    g_time.second  = 0;
    g_time.centisecond = 0;
    g_freeze_timer = 0;
    g_flow_pos = 0;
    g_flow_cnt = 0;

    /* 关闭所有 LED (高电平 = 灭) */
    LED0 = 1; LED1 = 1; LED2 = 1; LED3 = 1;
    LED4 = 1; LED5 = 1; LED6 = 1; LED7 = 1;

    /* 数码管清屏并显示 00-00-00 */
    LED_SMG_Clear();
    sw_UpdateDisplay();

#if STOPWATCH_DEBUG
    printf("[停止watch] 初始化完成, 状态=STOPPED\r\n");
#endif
}

/**
 * @brief   按键处理 — main 循环中调用
 *
 * 按键映射:
 *   KEY1 (PC9):  启动 ↔ 暂停
 *   KEY2 (PD2):  暂停时清零 (防误触)
 *   WK_UP (PA0): 运行中记次冻结
 *
 * KEY_Scan(mode=0) 内部有 10ms 去抖, 支持单按。
 */
void Stopwatch_KeyProcess(void)
{
    u8 key = KEY_Scan(0);  // 不支持连按

    switch (key) {

    case KEY1_PRES:
        /* KEY1: 运行→暂停, 冻结→解除并暂停, 停止/暂停→启动 */
        if (g_state == SW_RUNNING) {
            sw_SetState(SW_PAUSED);
            sw_UpdateDisplay();
            sw_OutputLog(SW_LOG_PAUSE, &g_time);
        } else if (g_state == SW_LAP_FROZEN) {
            /* 冻结中按 KEY1 → 解除冻结, 回到暂停 */
            g_freeze_timer = 0;
            sw_SetState(SW_PAUSED);
            sw_UpdateDisplay();
            sw_OutputLog(SW_LOG_PAUSE, &g_time);
        } else {
            /* 从 STOPPED 或 PAUSED 启动 */
            if (g_state == SW_STOPPED) {
                g_time.minute = 0;
                g_time.second = 0;
                g_time.centisecond = 0;
            }
            sw_SetState(SW_RUNNING);
            sw_UpdateDisplay();
            sw_OutputLog(SW_LOG_START, &g_time);
        }
        break;

    case KEY2_PRES:
        /* KEY2: 仅在暂停/停止状态允许清零 */
        if (g_state == SW_PAUSED || g_state == SW_STOPPED) {
            g_time.minute = 0;
            g_time.second = 0;
            g_time.centisecond = 0;
            g_freeze_timer = 0;
            sw_SetState(SW_STOPPED);
            sw_UpdateDisplay();
            sw_OutputLog(SW_LOG_RESET, &g_time);
        }
        break;

    case WKUP_PRES:
        /* WK_UP: 仅在运行中允许记次冻结 */
        if (g_state == SW_RUNNING) {
            g_lap = g_time;              // 保存当前时间
            g_freeze_timer = 0;          // 重置冻结计时
            sw_SetState(SW_LAP_FROZEN);
            sw_UpdateDisplay();          // 立即冻结显示
            sw_OutputLog(SW_LOG_LAP, &g_lap);
        }
        break;

    default:
        /* 0 = 无按键 */
        break;
    }
}

/**
 * @brief   定时器 Tick — TIM3 1ms 中断中调用
 *
 * 工作内容 (按优先级):
 *   1. 厘秒累加 (每 10ms 一次)
 *   2. 冻结倒计时 (达到 3s 自动恢复)
 *   3. LED 控制 (DS0/DS1 1Hz 闪烁 + DS2~DS7 流水灯)
 */
void Stopwatch_TimerTick(void)
{
    static u16 ms_counter = 0;

    ms_counter++;

    /* ── 1. 厘秒累加: 每 10ms 触发一次 ── */
    if (ms_counter >= TICK_10MS_MS) {
        ms_counter = 0;
        sw_UpdateTime();
    }

    /* ── 2. 冻结倒计时: 冻结满 3 秒后自动恢复 ── */
    if (g_state == SW_LAP_FROZEN) {
        g_freeze_timer++;
        if (g_freeze_timer >= LAP_FREEZE_MS) {
            g_freeze_timer = 0;
            sw_SetState(SW_RUNNING);
            sw_UpdateDisplay();      // 恢复显示实时时间
        }
    }

    /* ── 3. LED 控制 ── */
    sw_UpdateLEDs();
}

/* ---------- 状态/时间获取 (供外部调用) ---------- */
Stopwatch_State Stopwatch_GetState(void) { return g_state; }
Stopwatch_Time  Stopwatch_GetTime(void)  { return g_time; }

/* ================================================================ */
/*                         内部实现                                  */
/* ================================================================ */

/**
 * @brief   状态转移 (带调试输出)
 */
static void sw_SetState(Stopwatch_State new_state)
{
#if STOPWATCH_DEBUG
    static const char *state_names[] = {
        "STOPPED", "RUNNING", "PAUSED", "LAP_FROZEN"
    };
    /* 注意: state_names[4] 越界保护 */
    const char *old_name = (g_state < 4) ? state_names[g_state] : "???";
    const char *new_name = (new_state < 4) ? state_names[new_state] : "???";
    printf("[停止watch] 状态: %s → %s\r\n", old_name, new_name);
#endif
    g_state = new_state;
}

/**
 * @brief   厘秒累加和进位
 *
 * 厘秒满 100 → 秒 +1, 秒满 60 → 分 +1
 * 分 > 99 → 全部归零, 状态切回 STOPPED
 * 冻结状态下后台继续累加, 但不更新显示
 */
static void sw_UpdateTime(void)
{
    /* 仅在运行或冻结记次时累加 */
    if (g_state != SW_RUNNING && g_state != SW_LAP_FROZEN)
        return;

    /* ── 厘秒进位 ── */
    g_time.centisecond++;
    if (g_time.centisecond < CENTISECOND_PER_SEC) {
        goto done;
    }
    g_time.centisecond = 0;
    g_time.second++;

    if (g_time.second < SECOND_PER_MIN) {
        goto done;
    }
    g_time.second = 0;
    g_time.minute++;

    if (g_time.minute <= MINUTE_MAX) {
        goto done;
    }

    /* 超过 99 分 59 秒 99 厘秒 → 溢出归零停止 */
    g_time.minute = 0;
    g_time.second = 0;
    g_time.centisecond = 0;
    sw_SetState(SW_STOPPED);
    /* 继续到 done 更新显示 */

done:
    /* 非冻结状态时实时更新数码管 */
    if (g_state != SW_LAP_FROZEN) {
        sw_UpdateDisplay();
    }
}

/**
 * @brief   更新数码管显示缓冲区
 *
 * 8 位数码管布局:
 *   位0 位1  位2  位3 位4  位5  位6 位7
 *   [M十] [M个] [-] [S十] [S个] [-] [C十] [C个]
 *
 * 冻结状态显示 g_lap, 其他状态显示 g_time
 */
static void sw_UpdateDisplay(void)
{
    const Stopwatch_Time *dp;

    /* 选择显示源: 冻结中显示暂存值, 否则显示实时值 */
    dp = (g_state == SW_LAP_FROZEN) ? &g_lap : &g_time;

    /* 分: 十位 + 个位 */
    LED_SMG_WriteNum(0, dp->minute / 10);
    LED_SMG_WriteNum(1, dp->minute % 10);
    /* 分隔符 "-" */
    LED_SMG_WriteSeg(2, SEG_DASH);
    /* 秒: 十位 + 个位 */
    LED_SMG_WriteNum(3, dp->second / 10);
    LED_SMG_WriteNum(4, dp->second % 10);
    /* 分隔符 "-" */
    LED_SMG_WriteSeg(5, SEG_DASH);
    /* 厘秒: 十位 + 个位 */
    LED_SMG_WriteNum(6, dp->centisecond / 10);
    LED_SMG_WriteNum(7, dp->centisecond % 10);
}

/**
 * @brief   LED 控制 (每 1ms 调用一次)
 *
 * DS0/DS1 (LED0/LED1):   1Hz 交替闪烁
 *   偶秒 → DS0亮, DS1灭  ;  奇秒 → DS0灭, DS1亮
 *   暂停/停止时全灭
 *
 * DS2~DS7 (LED2~LED7):   0.5s 间隔单向流水灯
 *   始终运行, 不受暂停/冻结影响
 *   DS2 → DS3 → DS4 → DS5 → DS6 → DS7 → DS2 → ...
 */
static void sw_UpdateLEDs(void)
{
    /* ── DS0/DS1: 秒指示 1Hz 交替 ── */
    if (g_state == SW_RUNNING || g_state == SW_LAP_FROZEN) {
        /* 根据秒数奇偶交替: 偶秒 DS0 亮, 奇秒 DS1 亮 */
        if (g_time.second & 0x01) {
            /* 奇数秒 */
            LED0 = 1;   // 灭
            LED1 = 0;   // 亮
        } else {
            /* 偶数秒 */
            LED0 = 0;   // 亮
            LED1 = 1;   // 灭
        }
    } else {
        /* 暂停/停止: 全灭 */
        LED0 = 1;
        LED1 = 1;
    }

    /* ── DS2~DS7: 500ms 流水灯 (不受状态影响) ── */
    g_flow_cnt++;
    if (g_flow_cnt >= 500) {        // 500ms 到
        g_flow_cnt = 0;

        /* 先全部熄灭 DS2~DS7 */
        LED2 = 1; LED3 = 1;
        LED4 = 1; LED5 = 1;
        LED6 = 1; LED7 = 1;

        /* 移到下一位 */
        g_flow_pos++;
        if (g_flow_pos > 5)
            g_flow_pos = 0;

        /* 点亮当前位 (低电平亮) */
        switch (g_flow_pos) {
        case 0:  LED2 = 0;  break;
        case 1:  LED3 = 0;  break;
        case 2:  LED4 = 0;  break;
        case 3:  LED5 = 0;  break;
        case 4:  LED6 = 0;  break;
        case 5:  LED7 = 0;  break;
        }
    }
}

/**
 * @brief   串口日志输出
 *
 * 格式:
 *   启动: [秒表日志] 动作: 启动
 *   暂停: [秒表日志] 动作: 暂停, 暂停数值: 00-12-34
 *   清零: [秒表日志] 动作: 清零
 *   记次: [秒表日志] 动作: 暂存记次, 暂存时间: 00-12-34
 */
static void sw_OutputLog(Stopwatch_LogAction action, const Stopwatch_Time *t)
{
    char time_str[12];  // "MM-SS-CC" + null = 9 bytes

    if (t) {
        sprintf(time_str, "%02u-%02u-%02u",
                (unsigned)t->minute,
                (unsigned)t->second,
                (unsigned)t->centisecond);
    } else {
        time_str[0] = '\0';
    }

    switch (action) {
    case SW_LOG_START:
        printf("[秒表日志] 动作: 启动\r\n");
        break;

    case SW_LOG_PAUSE:
        printf("[秒表日志] 动作: 暂停, 暂停数值: %s\r\n", time_str);
        break;

    case SW_LOG_RESET:
        printf("[秒表日志] 动作: 清零\r\n");
        break;

    case SW_LOG_LAP:
        printf("[秒表日志] 动作: 暂存记次, 暂存时间: %s\r\n", time_str);
        break;
    }
}
