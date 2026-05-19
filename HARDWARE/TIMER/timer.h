#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

// 定时器信息结构体
typedef struct {
    u16 arr;             // 自动重装值
    u16 psc;             // 预分频值
    u32 tick_us;         // 每个硬件中断 tick = ? 微秒
    volatile u32 tick;   // 累计 tick 数(在 ISR 中递增)
} TIMER_TypeDef;

extern TIMER_TypeDef g_tim3;
extern TIMER_TypeDef g_tim4;

// 宏: 将毫秒转换为该定时器的 tick 数
#define TIMER_MS(pt, ms)  ((u32)((ms) * 1000 / (pt)->tick_us))

// 通用 API
u32  TIMER_GetTick(TIMER_TypeDef *pt);
u8   TIMER_IsElapsed(TIMER_TypeDef *pt, u32 last, u32 interval);
void TIMER_SetTim3Clock(u16 arr, u16 psc);

// 初始化
void TIM3_Init(u16 arr, u16 psc);
void TIM6_Init(u16 arr,u16 psc); //有1~8个定时器，TIM6是基本定时器，不能产生PWM波，只有更新事件，可以用来做系统滴答时钟
void TIM7_Init(u16 arr,u16 psc); //有1~8个定时器，TIM7是基本定时器，不能产生PWM波，只有更新事件，可以用来做系统滴答时钟

void TIM7_Init(u16 arr, u16 psc);
void TIM2_Cap_Init(u16 arr, u16 psc);

#endif
