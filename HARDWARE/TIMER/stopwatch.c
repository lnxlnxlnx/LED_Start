/**
 * @file    stopwatch.c
 * @brief   多功能秒表系统 — 实现
 *          基于正点原子 NANO STM32F103 开发板
 *
 * 架构:
 *   所有工作在 TIM3_IRQHandler (1ms 中断) 中完成:
 *     - 按键消抖 (20ms 软件消抖, 无需 KEY_Scan/delay_ms)
 *     - 厘秒累加 + 数码管显示刷新
 *     - 冻结 3 秒自动恢复
 *     - LED 控制 (DS0/DS1 1Hz 交替, DS2~DS7 流水灯)
 *   主循环仅 delay_ms(20)
 *
 * 调试:
 *   串口 115200 输出 [停止watch] 状态变化 + [秒表日志] 动作
 *   如果串口无输出, 检查 TIM3 是否运行 (1ms 中断)
 *
 * 接线 (开发板已有):
 *   数码管: 74HC138(PC10/11/12) + 74HC595(PB3/4/5)
 *   LED:   PC0~PC7 (低电平亮)
 *   KEY1:  PC9 (按下=0)    KEY2: PD2 (按下=0)    WK_UP: PA0 (按下=1)
 *   USART: PA9(TX) PA10(RX), 115200-8N1
 */

#include "stopwatch.h"
#include "key.h"       // 仅用 KEY1/KEY2/WK_UP 宏 (直接读 GPIO)
#include "led.h"       // LED0~LED7 宏
#include "smg.h"       // LED_SMG_WriteNum/LED_SMG_WriteSeg/LED_SMG_Clear
#include <stdio.h>     // printf()

/* ======================== 调试开关 ======================== */
#define STOPWATCH_DEBUG    1       // 1=串口输出状态变化, 0=关闭

/* ======================== 常量 ======================== */
#define TICK_10MS_MS        10      // 厘秒间隔 (10ms)
#define CENTISECOND_PER_SEC 100     // 100 厘秒 = 1 秒
#define SECOND_PER_MIN      60      // 60 秒 = 1 分
#define MINUTE_MAX          99      // 最大 99 分

#define LAP_FREEZE_MS       3000    // 记次冻结 3 秒
#define KEY_DEBOUNCE_MS     20      // 按键消抖时间

#define SEG_DASH            0x02    // 横线 "-" 段码 (共阴: 仅 g 段亮)

/* ======================== 按键索引 ======================== */
#define K_IDX_KEY1   0
#define K_IDX_KEY2   1
#define K_IDX_WKUP   2
#define KEY_COUNT    3

/* ======================== 按键消抖结构 ======================== */
typedef struct {
    u8 raw;          // 上次原始电平
    u8 stable;       // 消抖后稳定电平 (1=按下, 0=松开)
    u8 count;        // 稳定持续时间计数
    u8 trigger;      // 1 = 刚检测到按下 (上升沿), 处理后清 0
} KeyDebounce;

/* ======================== 全局变量 ======================== */
static Stopwatch_State  g_state = SW_STOPPED;
static Stopwatch_Time   g_time  = {0, 0, 0};
static Stopwatch_Time   g_lap   = {0, 0, 0};

static u32 g_freeze_timer = 0;      // 冻结倒计时 (ms)
static u8  g_flow_pos     = 0;      // 流水灯当前位置
static u16 g_flow_cnt     = 0;      // 流水灯计数器

static KeyDebounce g_key[KEY_COUNT]; // 按键消抖状态

/* ======================== 静态函数 ======================== */
static void sw_SetState(Stopwatch_State new_state);
static void sw_UpdateTime(void);
static void sw_UpdateDisplay(void);
static void sw_UpdateLEDs(void);
static void sw_ReadKeys(void);
static void sw_ProcessKey(u8 idx);
static void sw_OutputLog(const char *action, const Stopwatch_Time *t);

/* ================================================================ */
/*                         公开 API                                  */
/* ================================================================ */

void Stopwatch_Init(void)
{
    u8 i;

    g_state = SW_STOPPED;
    g_time.minute = 0; g_time.second = 0; g_time.centisecond = 0;
    g_freeze_timer = 0;
    g_flow_pos = 0; g_flow_cnt = 0;

    /* 按键消抖结构清零 */
    for (i = 0; i < KEY_COUNT; i++) {
        g_key[i].raw = 0;
        g_key[i].stable = 0;
        g_key[i].count = 0;
        g_key[i].trigger = 0;
    }

    /* LED 全灭 (高电平 = 灭) */
    LED0 = 1; LED1 = 1; LED2 = 1; LED3 = 1;
    LED4 = 1; LED5 = 1; LED6 = 1; LED7 = 1;

    /* 数码管显示 00-00-00 */
    LED_SMG_Clear();
    sw_UpdateDisplay();

#if STOPWATCH_DEBUG
    printf("[停止watch] 初始化完成\r\n");
#endif
}

/**
 * @brief   定时器 Tick — TIM3 1ms 中断中调用
 *
 * 每 1ms:
 *   - 按键消抖采样
 *   每 10ms:
 *   - 厘秒累加
 *   每 1ms:
 *   - 冻结倒计时判断
 *   - LED 控制
 */
void Stopwatch_TimerTick(void)
{
    static u16 ms_counter = 0;

    /* ── 1. 按键消抖 (每 1ms) ── */
    sw_ReadKeys();

    /* ── 2. 厘秒累加 (每 10ms) ── */
    ms_counter++;
    if (ms_counter >= TICK_10MS_MS) {
        ms_counter = 0;
        sw_UpdateTime();
    }

    /* ── 3. 冻结倒计时 (每 1ms) ── */
    if (g_state == SW_LAP_FROZEN) {
        g_freeze_timer++;
        if (g_freeze_timer >= LAP_FREEZE_MS) {
            g_freeze_timer = 0;
            sw_SetState(SW_RUNNING);
            sw_UpdateDisplay();
        }
    }

    /* ── 4. LED 控制 (每 1ms) ── */
    sw_UpdateLEDs();
}

Stopwatch_State Stopwatch_GetState(void) { return g_state; }
Stopwatch_Time  Stopwatch_GetTime(void)  { return g_time; }

/* ================================================================ */
/*                         内部实现                                  */
/* ================================================================ */

/* ---------- 状态转移 ---------- */
static void sw_SetState(Stopwatch_State new_state)
{
#if STOPWATCH_DEBUG
    static const char *name[] = {"STOPPED","RUNNING","PAUSED","LAP_FROZEN"};
    const char *old_s = (g_state < 4) ? name[g_state] : "???";
    const char *new_s = (new_state < 4) ? name[new_state] : "???";
    if (g_state != new_state)
        printf("[停止watch] %s → %s\r\n", old_s, new_s);
#endif
    g_state = new_state;
}

/* ---------- 厘秒累加 + 进位 ---------- */
static void sw_UpdateTime(void)
{
    if (g_state != SW_RUNNING && g_state != SW_LAP_FROZEN)
        return;

    g_time.centisecond++;
    if (g_time.centisecond < CENTISECOND_PER_SEC) goto done;
    g_time.centisecond = 0;
    g_time.second++;

    if (g_time.second < SECOND_PER_MIN) goto done;
    g_time.second = 0;
    g_time.minute++;

    if (g_time.minute <= MINUTE_MAX) goto done;

    /* 溢出: 归零停止 */
    g_time.minute = 0; g_time.second = 0; g_time.centisecond = 0;
    sw_SetState(SW_STOPPED);

done:
    if (g_state != SW_LAP_FROZEN)
        sw_UpdateDisplay();
}

/* ---------- 数码管显示 ---------- */
static void sw_UpdateDisplay(void)
{
    const Stopwatch_Time *dp;
    dp = (g_state == SW_LAP_FROZEN) ? &g_lap : &g_time;

    LED_SMG_WriteNum(0, dp->minute / 10);
    LED_SMG_WriteNum(1, dp->minute % 10);
    LED_SMG_WriteSeg(2, SEG_DASH);
    LED_SMG_WriteNum(3, dp->second / 10);
    LED_SMG_WriteNum(4, dp->second % 10);
    LED_SMG_WriteSeg(5, SEG_DASH);
    LED_SMG_WriteNum(6, dp->centisecond / 10);
    LED_SMG_WriteNum(7, dp->centisecond % 10);
}

/* ---------- LED 控制 ---------- */
static void sw_UpdateLEDs(void)
{
    /* DS0/DS1: 1Hz 交替 (运行/冻结时) */
    if (g_state == SW_RUNNING || g_state == SW_LAP_FROZEN) {
        if (g_time.second & 0x01) {
            LED0 = 1; LED1 = 0;  // 奇秒: DS1亮
        } else {
            LED0 = 0; LED1 = 1;  // 偶秒: DS0亮
        }
    } else {
        LED0 = 1; LED1 = 1;      // 暂停/停止: 全灭
    }

    /* DS2~DS7: 500ms 流水灯 (不受状态影响) */
    g_flow_cnt++;
    if (g_flow_cnt >= 500) {
        g_flow_cnt = 0;
        LED2 = 1; LED3 = 1; LED4 = 1; LED5 = 1; LED6 = 1; LED7 = 1;
        g_flow_pos++;
        if (g_flow_pos > 5) g_flow_pos = 0;
        switch (g_flow_pos) {
            case 0: LED2 = 0; break;
            case 1: LED3 = 0; break;
            case 2: LED4 = 0; break;
            case 3: LED5 = 0; break;
            case 4: LED6 = 0; break;
            case 5: LED7 = 0; break;
        }
    }
}

/* ---------- 按键消抖 + 处理 (每 1ms) ---------- */
static void sw_ReadKeys(void)
{
    u8 raw[KEY_COUNT];
    u8 i;

    /* 读原始电平, 统一转为 1=按下 */
    raw[K_IDX_KEY1] = !KEY1;    // PC9 低电平按下
    raw[K_IDX_KEY2] = !KEY2;    // PD2 低电平按下
    raw[K_IDX_WKUP] = WK_UP;    // PA0 高电平按下

    for (i = 0; i < KEY_COUNT; i++) {
        if (raw[i] == g_key[i].raw) {
            /* 电平持续稳定 → 计数累加 */
            if (g_key[i].count < KEY_DEBOUNCE_MS) {
                g_key[i].count++;
                if (g_key[i].count >= KEY_DEBOUNCE_MS) {
                    /* 消抖完成: 检测上升沿 (松开→按下) */
                    if (g_key[i].stable == 0 && raw[i] == 1) {
                        g_key[i].trigger = 1;
                    }
                    g_key[i].stable = raw[i];
                }
            }
        } else {
            /* 电平跳变 → 重置计数器 */
            g_key[i].raw = raw[i];
            g_key[i].count = 0;
        }
    }

    /* ── 处理已消抖的按键 ── */
    for (i = 0; i < KEY_COUNT; i++) {
        if (g_key[i].trigger) {
            g_key[i].trigger = 0;
            sw_ProcessKey(i);
        }
    }
}

/**
 * @brief   按键动作处理 (在 ISR 中调用)
 *
 * KEY1:  运行→暂停, 冻结→暂停, 停止/暂停→启动
 * KEY2:  暂停/停止→清零 (防误触)
 * WK_UP: 运行中→记次冻结
 */
static void sw_ProcessKey(u8 idx)
{
    switch (idx) {

    case K_IDX_KEY1:
        if (g_state == SW_RUNNING) {
            sw_SetState(SW_PAUSED);
            sw_UpdateDisplay();
            sw_OutputLog("暂停", &g_time);
        } else if (g_state == SW_LAP_FROZEN) {
            g_freeze_timer = 0;
            sw_SetState(SW_PAUSED);
            sw_UpdateDisplay();
            sw_OutputLog("暂停", &g_time);
        } else {
            /* STOPPED / PAUSED → RUNNING */
            if (g_state == SW_STOPPED) {
                g_time.minute = 0; g_time.second = 0; g_time.centisecond = 0;
            }
            sw_SetState(SW_RUNNING);
            sw_UpdateDisplay();
            sw_OutputLog("启动", &g_time);
        }
        break;

    case K_IDX_KEY2:
        /* 仅暂停/停止时可清零 */
        if (g_state == SW_PAUSED || g_state == SW_STOPPED) {
            g_time.minute = 0; g_time.second = 0; g_time.centisecond = 0;
            g_freeze_timer = 0;
            sw_SetState(SW_STOPPED);
            sw_UpdateDisplay();
            sw_OutputLog("清零", &g_time);
        }
        break;

    case K_IDX_WKUP:
        /* 仅运行时可记次 */
        if (g_state == SW_RUNNING) {
            g_lap = g_time;
            g_freeze_timer = 0;
            sw_SetState(SW_LAP_FROZEN);
            sw_UpdateDisplay();
            sw_OutputLog("暂存记次", &g_lap);
        }
        break;

    default:
        break;
    }
}

/* ---------- 串口日志 ---------- */
static void sw_OutputLog(const char *action, const Stopwatch_Time *t)
{
    if (t) {
        printf("[秒表日志] 动作: %s, 时间: %02u-%02u-%02u\r\n",
               action,
               (unsigned)t->minute,
               (unsigned)t->second,
               (unsigned)t->centisecond);
    } else {
        printf("[秒表日志] 动作: %s\r\n", action);
    }
}
