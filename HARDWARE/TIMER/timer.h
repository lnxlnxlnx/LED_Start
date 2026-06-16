/**
 * @file    timer.h
 * @brief   定时器驱动头文件
 */

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include <stm32f10x_tim.h>

typedef struct {
    u16 arr;             // 自动重装值
    u16 psc;             // 预分频值
    u32 tick_us;         // 每个硬件 tick 对应多少微秒
    volatile u32 tick;   // 累计 tick 数 (ISR 中递增)
} TIMER_TypeDef;

extern TIMER_TypeDef g_tim3;
extern TIMER_TypeDef g_tim4;

#define TIMER_MS(pt, ms)  ((u32)((ms) * 1000 / (pt)->tick_us))

u32  TIMER_GetTick(TIMER_TypeDef *pt);
u8   TIMER_IsElapsed(TIMER_TypeDef *pt, u32 last, u32 interval);
void TIMER_SetTim3Clock(u16 arr, u16 psc);

void TIM3_Init(u16 arr, u16 psc);
void My_TIM3_Init(u16 arr, u16 psc);
void TIM4_Init(u16 arr, u16 psc);
void TIM6_Init(u16 arr, u16 psc);
void TIM7_Init(u16 arr, u16 psc);
void TIM2_Cap_Init(u16 arr, u16 psc);

#endif
