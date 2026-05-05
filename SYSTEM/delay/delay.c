#include "stm32f10x.h"   // Device header

/**
 * @brief us级Delay函数(精确)
 * @param n 延时 n*1us，n范围[0, 233016]
 * @retval void
*/
void delay_us(uint32_t n)  
{
    SysTick->LOAD=72*n;          //装载计数值，因为时钟 72M，72 次在 1μs
    SysTick->VAL=0x00;          //清空计数器
    SysTick->CTRL=0x00000005;   //时钟来源设为为 HCLK(72M)，打开定时器
    while(! (SysTick->CTRL&0x00010000)); //等待计数到 0
    SysTick->CTRL=0x00000004;   //关闭定时器
}

/**
 * @brief ms级延时函数(精确)
 * @param n 延时 n*1ms，n范围[0, 233]
 * @retval void
*/
void delay_ms(uint32_t n)  
{
    SysTick->LOAD=72000*n;      //装载计数值，因为时钟 72000 kHz，72000次在 1ms
    SysTick->VAL=0x00;          //清空计数器
    SysTick->CTRL=0x00000005;   //时钟来源设为为 HCLK(72M)，打开定时器
    while(! (SysTick->CTRL&0x00010000)); //等待计数到 0
    SysTick->CTRL=0x00000004;   //关闭定时器
}

/**
 * @brief s级延时函数(精确)
 * @param n 延时 n*1s，n范围[0, 858993459]
 * @retval void
*/
void delay_s(uint32_t n)  
{
    uint32_t Systick_counter;  
    for ( Systick_counter = 0; Systick_counter < n*5; Systick_counter++)
    {
        delay_ms(200);
    }
}