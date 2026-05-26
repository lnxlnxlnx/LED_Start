#include "timer.h"
#include "led.h"
#include "beep.h"
#include "ul_thread.h"
#include "smg.h"
#include "elog.h"
#include "remote.h"
#include <stdint.h>

// 全局定时器实例
TIMER_TypeDef g_tim3 = {0};
TIMER_TypeDef g_tim4 = {0};

// 计算 tick_us = (arr+1)*(psc+1) / 72  (假设72MHz时钟)
/**
 * @description: 用于计算定时器每个tick的时间(单位:微秒)，基于定时器的自动重装值(arr)和预分频值(psc)。这个函数在初始化定时器时被调用，以便正确设置定时器的tick计数。
 * @param {u16} arr
 * @param {u16} psc
 * @return {*}
 */
static u32 calc_tick_us(u16 arr, u16 psc)
{
    return (u32)(arr + 1) * (psc + 1) / 72;
}

// ── 初始化 ──────────────────────────────────────────

void TIM3_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 1;	//TIM3时钟使能    
	TIM3->ARR = arr;  	//设定计数器自动重装值 
	TIM3->PSC = psc;  	//预分频器设置
	TIM3->DIER |= 1 << 0;   //允许更新中断				
	TIM3->CR1 |= 0x01;    //使能定时器3
	MY_NVIC_Init(0, 3, TIM3_IRQn, 2);//抢占0，子优先级3，组2		

}

void TIM4_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 2;    // TIM4时钟使能
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
	RCC->APB1ENR |= 1 << 4;	//TIM6时钟使能    
	TIM6->ARR = arr;  	//设定计数器自动重装值 
	TIM6->PSC = psc;  	//预分频器设置
	TIM6->DIER |= 1 << 0;   //允许更新中断		
    TIM6->CR1 |= 0x01;
	MY_NVIC_Init(0, 3, TIM6_IRQn, 2);//抢占0，子优先级3，组2	
	//这里应该是TIM6_DAC_IRQn
}

void TIM7_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 5;	//TIM7时钟使能    
	TIM7->ARR = arr;  	//设定计数器自动重装值 
	TIM7->PSC = psc;  	//预分频器设置
	TIM7->DIER |= 1 << 0;   //允许更新中断		
    TIM7->CR1 |= 0x01;
    MY_NVIC_Init(0, 3, TIM7_IRQn, 2);
}

// ── TIM2 输入捕获 ────────────────────────────────────

void TIM2_Cap_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 0;     // TIM2 时钟使能
    RCC->APB2ENR |= 1 << 2;     // PORTA时钟

	GPIOA->CRL &= 0XFFFFFFF0;	//PA0 清除之前设置  
	GPIOA->CRL |= 0X00000008;	//PA0 输入   
	GPIOA->ODR |= 0 << 0;		//PA0 下拉

    TIM2->ARR = arr;
    TIM2->PSC = psc;

	TIM2->CCMR1 |= 1 << 0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2->CCMR1 |= 1 << 4; 		//IC1F=0001 配置输入滤波器 以Fck_int采样，2个事件后有效
	TIM2->CCMR1 |= 0 << 10; 	//IC2PS=00 	配置输入分频,不分频 

	TIM2->CCER |= 0 << 1; 		//CC1P=0	上升沿捕获
	TIM2->CCER |= 1 << 0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM2->DIER |= 1 << 1;   	//允许捕获中断				
	TIM2->DIER |= 1 << 0;   	//允许更新中断	
	TIM2->CR1 |= 0x01;    	//使能定时器2
	MY_NVIC_Init(2, 0, TIM2_IRQn, 2);//抢占2，子优先级0，组2

}

// ── 通用 API ────────────────────────────────────────

/**
 * @description: 给定时器的结构体设置时钟参数，并计算每个tick的时间(单位:微秒)。这个函数通常在定时器初始化后被调用，以确保定时器的tick计数正确反映实际时间。
 * @param {u16} arr
 * @param {u16} psc
 * @return {*}
 */
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

// ── 输入捕获状态 ──────────────────────────────────

u8  TIM2CH1_CAPTURE_STA = 0;
u16 TIM2CH1_CAPTURE_VAL;

uint16_t TIM2_SIG_HIGH_TIME = 0;

// ── ISR ─────────────────────────────────────────────

void TIM2_IRQHandler(void)
{
	u16 tsr;
	tsr = TIM2->SR;
	if ((TIM2CH1_CAPTURE_STA & 0X80) == 0)//还未成功捕获	
    {
		if (tsr & 0X01)//溢出
        {
			if (TIM2CH1_CAPTURE_STA & 0X40)//已经捕获到高电平了
            {
				if ((TIM2CH1_CAPTURE_STA & 0X3F) == 0X3F)//高电平太长了
                {
					TIM2CH1_CAPTURE_STA |= 0X80;//标记成功捕获了一次
                    TIM2CH1_CAPTURE_VAL = 0XFFFF;
                }
                else TIM2CH1_CAPTURE_STA++;
            }
        }
		if (tsr & 0x02)//捕获1发生捕获事件
        {
			if (TIM2CH1_CAPTURE_STA & 0X40)		//捕获到一个下降沿 		
            {
				TIM2CH1_CAPTURE_STA |= 0X80;		//标记成功捕获到一次高电平脉宽
				TIM2CH1_CAPTURE_VAL = TIM2->CCR1;	//获取当前的捕获值.
				TIM2->CCER &= ~(1 << 1);			//CC1P=0 设置为上升沿捕获
            }
			else  								//还未开始,第一次捕获上升沿
            {
                TIM2CH1_CAPTURE_VAL = 0;
				TIM2CH1_CAPTURE_STA = 0X40;		//标记捕获到了上升沿
				TIM2->CNT = 0;					//计数器清空
				TIM2->CCER |= 1 << 1; 				//CC1P=1 设置为下降沿捕获 
            }
        }
    }
	TIM2->SR = 0;//清除中断标志位 	    
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
	if (TIM2CH1_CAPTURE_STA & 0X80) //成功捕获到了一次高电平
    {
		temp = TIM2CH1_CAPTURE_STA & 0X3F;	// 低 6 位存溢出次数
		temp *= 65536;					//溢出时间总和
		temp += TIM2CH1_CAPTURE_VAL;		//得到总的高电平时间
		TIM2_SIG_HIGH_TIME = temp/1000;
		printf("HIGH:%d us	, %d ms\r\n", temp, TIM2_SIG_HIGH_TIME);	//打印总的高点平时间
		TIM2CH1_CAPTURE_STA = 0;			//开启下一次捕获
    }
}

// ── TIM3 ─────────────────────────────────────────────

extern void remote_irq_func(void);
extern void led_irq_func(void);
extern void adc_irq_func(void);

#define USE_LED      0
#define USE_REMOTE   1
#define USE_IC_UPDATE 0
#define USE_ADC_REF  0

void TIM3_IRQHandler(void)
{
    // 只在更新中断(定时器溢出)时加 tick，捕获中断不计
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
extern void remote_irq_func(void);
#define USE_REMOTE_SMG_IRQ 0
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 0X0001)
    {
        g_tim4.tick++;
		#if USE_REMOTE
		LED_SMG_Scan();
		#endif
		#if USE_REMOTE_SMG_IRQ	
        remote_smg_irq_func();
		#endif
	}

    TIM4->SR &= ~(1 << 0);
}

#ifdef USE_REMOTE
#undef USE_REMOTE
#endif
