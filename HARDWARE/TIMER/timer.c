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

TIMER_TypeDef g_tim1 = { 0 };
TIMER_TypeDef g_tim2 = { 0 };
TIMER_TypeDef g_tim3 = { 0 };
TIMER_TypeDef g_tim4 = { 0 };
TIMER_TypeDef g_tim5 = { 0 };
TIMER_TypeDef g_tim6 = { 0 };
TIMER_TypeDef g_tim7 = { 0 };
TIMER_TypeDef g_tim8 = { 0 };

// 底层拼接，不展开参数
#define _TIMER_SET_CLOCK(x,arr,psc) do{ \
    g_tim##x.arr = arr; \
    g_tim##x.psc = psc; \
    g_tim##x.tick_us = calc_tick_us(arr, psc); \
    g_tim##x.tick = 0; \
}while(0)

// 外层中转，先展开 x、arr、psc 再拼接
#define TIMER_SET_CLOCK(x,arr,psc) _TIMER_SET_CLOCK(x,arr,psc)  // 这样写的好处是，调用时传入的 arr、psc 可以是表达式，而不是单纯的数值常量。又或者x是TIM_NUM这种宏定义，那就会先展开成具体的数字，再进行内层宏的拼接。

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

    TIMER_SetTim3Clock(arr, psc);  // 初始化 g_tim3 时间基准参数
}

static void NVIC_TIM3_Init(void)
{
    //MY_NVIC_Init(0, 3, TIM3_IRQn, 2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;     //这几个NVIC结构体的成员都是uint8_t类型的，所以直接赋值就行了，不需要强制类型转换。
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void My_TIM3_Init(u16 arr, u16 psc)
{
    // TIM3_Init(TIM3_ONE_SECOND_COUNT, 72 - 1);
    // 1. 配置时基结构体
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 开启TIM3时钟
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 只给滤波、死区用，不影响定时器计数器计数速度。
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //2. 配置中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //3. 配置NVIC--中断管理
    //MY_NVIC_Init(0, 3, TIM3_IRQn, 2);
    NVIC_TIM3_Init();

    //4. 启动定时器
    TIM_Cmd(TIM3, ENABLE);

    //5. 初始化 g_tim3 时间基准参数
    //TIMER_SetTim3Clock(arr, psc);  // 初始化 g_tim3 时间基准参数
    TIMER_SET_CLOCK(3, arr, psc);  // 使用宏简化代码
}

void My_TIM3_PWM_Init(u16 arr, u16 psc)
{
    My_TIM3_Init(arr, psc);
    GPIO_InitTypeDef GPIO_InitStructure;
    //1.  配置 GPIOC 时钟和 PC6 复用推挽输出
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //2. 配置 TIM3 的 PWM 模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM 模式 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比为
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 输出极性低
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能预装载寄存器，更新事件时才会把 CCR1 的值加载到比较寄存器中，这样就不会出现占空比设置后立即生效导致的毛刺问题。

    // TIMER_SET_CLOCK(3, arr, psc);  // 使用宏简化代码
}

void TIM4_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 2;    // TIM4 时钟使能
    TIM4->ARR = arr;
    TIM4->PSC = psc;
    TIM4->DIER |= 1 << 0;
    TIM4->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM4_IRQn, 2);

    TIMER_SET_CLOCK(4, arr, psc);  // 使用宏简化代码
}

void TIM6_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 4;    // TIM6 时钟使能
    TIM6->ARR = arr;
    TIM6->PSC = psc;
    TIM6->DIER |= 1 << 0;
    TIM6->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM6_IRQn, 2);
    TIMER_SET_CLOCK(6, arr, psc);  // 使用宏简化代码
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

u32 TIMER_GetTick(TIMER_TypeDef* pt)
{
    return pt->tick;
}

u8 TIMER_IsElapsed(TIMER_TypeDef* pt, u32 last, u32 interval)
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
extern void adc_irq_func(TIMER_TypeDef* callback_timer);
extern void led_pwm_func(void);

#define USE_LED      1
#define USE_PWM_TEST   1
#define USE_IC_UPDATE 0
#define USE_ADC_REF  0
#define USE_REMOTE   0

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & 0X01) {
        g_tim3.tick++;

#if USE_REMOTE
        remote_irq_func();
#endif
#if USE_LED
        led_irq_func();
#endif
#if USE_PWM_TEST
        led_pwm_func();
#endif
#if USE_IC_UPDATE
        TIM2_Input_Capture_Update();
#endif
#if USE_ADC_REF
        adc_irq_func(NULL);  // 使用默认的 g_tim3 作为时间基准
#endif
    }
    TIM3->SR = 0;
}

extern u16 TIM3_ONE_SECOND_COUNT;

// ── TIM4 ─────────────────────────────────────────────
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 0X0001)
    {
        g_tim4.tick++;
        //adc_irq_func(&g_tim4);
    }
    TIM4->SR &= ~(1 << 0);
}

#ifdef USE_REMOTE
#undef USE_REMOTE
#endif
