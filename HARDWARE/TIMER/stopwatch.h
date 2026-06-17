/**
 * @file    stopwatch.h
 * @brief   多功能秒表系统 — 头文件
 *          正点原子 NANO STM32 开发板
 *          实验考试: 数码管秒表 + 记次 + LED + 串口日志
 *          按键在 TIM3 中断中消抖处理 (20ms 软件消抖)
 */

#ifndef __STOPWATCH_H
#define __STOPWATCH_H

#include "sys.h"

/* ======================== 秒表状态枚举 ======================== */
typedef enum {
    SW_STOPPED    = 0,    // 停止 (上电/清零后)
    SW_RUNNING    = 1,    // 运行中
    SW_PAUSED     = 2,    // 已暂停
    SW_LAP_FROZEN = 3     // 记次冻结 (显示冻结, 后台继续计时)
} Stopwatch_State;

/* ======================== 秒表时间结构 ======================== */
typedef struct {
    u16 minute;         // 分 (0~99)
    u8  second;         // 秒 (0~59)
    u8  centisecond;    // 厘秒 (0~99, 每厘秒 = 10ms)
} Stopwatch_Time;

/* ======================== 公开 API ======================== */

/**
 * @brief   秒表模块初始化 (数码管显示 00-00-00, LED 全灭)
 * @note    在 main 开头, 定时器启动前调用一次
 */
void Stopwatch_Init(void);

/**
 * @brief   定时器 Tick — 由 TIM3_IRQHandler (1ms) 调用
 *          按键消抖 + 厘秒累加 + 冻结倒计时 + LED 控制
 */
void Stopwatch_TimerTick(void);

/**
 * @brief   获取当前秒表状态
 */
Stopwatch_State Stopwatch_GetState(void);

/**
 * @brief   获取当前计时值
 */
Stopwatch_Time  Stopwatch_GetTime(void);

#endif
