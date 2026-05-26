/**
 * @file    timer.c
 * @brief   定时器驱动
 */

#include "timer.h"
#include "led.h"
#include "beep.h"
#include "ul_thread.h"
#include "smg.h"
#include "elog.h"
#include "remote.h"
#include <stdint.h>

TIMER_TypeDef g_tim3 = {0};
TIMER_TypeDef g_tim4 = {0};

static u32 calc_tick_us(u16 arr, u16 psc)
{
    return (u32)(arr + 1) * (psc + 1) / 72;
}

void TIM3_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 1;    // TIM3 时钟使能
    TIM3->ARR = arr;
    TIM3->PSC = psc;
    TIM3->DIER |= 1 << 0;      // 允许更新中断
    TIM3->CR1 |= 0x01;         // 使能定时器
    MY_NVIC_Init(0, 3, TIM3_IRQn, 2);
}

void TIM4_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 2;    // TIM4 时钟使能
    TIM4->ARR = arr;
    TIM4->PSC = psc;
    TIM4->DIER |= 1 << 0;
    TIM4->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM4_IRQn, 2);

    g_tim4.arr = arr;
    g_tim4.psc = psc;
    g_tim4.tick_us = calc_tick_us(arr, psc);
    g_tim4.tick = 0;
}

void TIM6_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 4;    // TIM6 时钟使能
    TIM6->ARR = arr;
    TIM6->PSC = psc;
    TIM6->DIER |= 1 << 0;
    TIM6->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM6_IRQn, 2);
}

void TIM7_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 5;    // TIM7 时钟使能
    TIM7->ARR = arr;
    TIM7->PSC = psc;
    TIM7->DIER |= 1 << 0;
    TIM7->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM7_IRQn, 2);
}

void TIM2_Cap_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 0;    // TIM2 时钟使能
    RCC->APB2ENR |= 1 << 2;    // PORTA 时钟

    GPIOA->CRL &= 0XFFFFFFF0;
    GPIOA->CRL |= 0X00000008;  // PA0 输入
    GPIOA->ODR |= 0 << 0;      // PA0 下拉

    TIM2->ARR = arr;
    TIM2->PSC = psc;

    TIM2->CCMR1 |= 1 << 0;     // CC1S=01  IC1 映射到 TI1
    TIM2->CCMR1 |= 1 << 4;     // IC1F=0001 输入滤波器
    TIM2->CCMR1 |= 0 << 10;    // IC2PS=00  不分频

    TIM2->CCER |= 0 << 1;      // CC1P=0 上升沿捕获
    TIM2->CCER |= 1 << 0;      // CC1E=1  允许捕获

    TIM2->DIER |= 1 << 1;      // 允许捕获中断
    TIM2->DIER |= 1 << 0;      // 允许更新中断
    TIM2->CR1 |= 0x01;         // 使能定时器
    MY_NVIC_Init(2, 0, TIM2_IRQn, 2);
}

void TIMER_SetTim3Clock(u16 arr, u16 psc)
{
    g_tim3.arr = arr;
    g_tim3.psc = psc;
    g_tim3.tick_us = calc_tick_us(arr, psc);
    g_tim3.tick = 0;
}

u32 TIMER_GetTick(TIMER_TypeDef *pt)
{
    return pt->tick;
}

u8 TIMER_IsElapsed(TIMER_TypeDef *pt, u32 last, u32 interval)
{
    return (TIMER_GetTick(pt) - last) >= interval;
}

u8  TIM2CH1_CAPTURE_STA = 0;
u16 TIM2CH1_CAPTURE_VAL;
uint16_t TIM2_SIG_HIGH_TIME = 0;

void TIM2_IRQHandler(void)
{
    u16 tsr;
    tsr = TIM2->SR;
    if ((TIM2CH1_CAPTURE_STA & 0X80) == 0)
    {
        if (tsr & 0X01)
        {
            if (TIM2CH1_CAPTURE_STA & 0X40)
            {
                if ((TIM2CH1_CAPTURE_STA & 0X3F) == 0X3F)
                {
                    TIM2CH1_CAPTURE_STA |= 0X80;
                    TIM2CH1_CAPTURE_VAL = 0XFFFF;
                }
                else TIM2CH1_CAPTURE_STA++;
            }
        }
        if (tsr & 0x02)
        {
            if (TIM2CH1_CAPTURE_STA & 0X40)
            {
                TIM2CH1_CAPTURE_STA |= 0X80;
                TIM2CH1_CAPTURE_VAL = TIM2->CCR1;
                TIM2->CCER &= ~(1 << 1);
            }
            else
            {
                TIM2CH1_CAPTURE_VAL = 0;
                TIM2CH1_CAPTURE_STA = 0X40;
                TIM2->CNT = 0;
                TIM2->CCER |= 1 << 1;
            }
        }
    }
    TIM2->SR = 0;
}

void TIM2_Input_Capture_Update(void)
{
    static uint32_t temp = 0;
    static uint8_t t = 0;
    t++;
    if (t >= TIMER_MS(&g_tim3, 190))
    {
        t = 0;
        LED0 = !LED0;
    }
    if (TIM2CH1_CAPTURE_STA & 0X80)
    {
        temp = TIM2CH1_CAPTURE_STA & 0X3F;
        temp *= 65536;
        temp += TIM2CH1_CAPTURE_VAL;
        TIM2_SIG_HIGH_TIME = temp / 1000;
        printf("HIGH:%d us , %d ms\r\n", temp, TIM2_SIG_HIGH_TIME);
        TIM2CH1_CAPTURE_STA = 0;
    }
}

// ── TIM3 ─────────────────────────────────────────────

extern void remote_irq_func(void);
extern void led_irq_func(void);
extern void adc_irq_func(void);

#define USE_LED      0
#define USE_REMOTE   0
#define USE_IC_UPDATE 0
#define USE_ADC_REF  0

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & 0X01)
        g_tim3.tick++;

#if USE_REMOTE
    remote_irq_func();
#endif
#if USE_LED
    led_irq_func();
#endif
#if USE_IC_UPDATE
    TIM2_Input_Capture_Update();
#endif
#if USE_ADC_REF
    adc_irq_func();
#endif
    TIM3->SR = 0;
}

extern u16 TIM3_ONE_SECOND_COUNT;

// ── TIM4 ─────────────────────────────────────────────

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 0X0001)
    {
        g_tim4.tick++;
        LED_SMG_Scan();
        LED_SMG_AutoCycle();
    }
    TIM4->SR &= ~(1 << 0);
}

#ifdef USE_REMOTE
#undef USE_REMOTE
#endif
