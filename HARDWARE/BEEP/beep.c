#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
	    
//蜂鸣器初始化
#include "stm32f10x.h"  // 标准库必须包含

void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 1. 开 GPIOB 时钟（对应原来 RCC->APB2ENR |= 1<<3;）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 配置 PB8 为推挽输出（对应原来 CRH 配置）
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;    // 推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    // 速度50M
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. PB8 初始输出高电平（对应原来 ODR |= 1<<8;）
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

// void BEEP_Init(void)
// {
 	
//   RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   
//   GPIOB->CRH&=0XFFFFFFF0; 
//   GPIOB->CRH|=0X00000003;//PB8推挽输出   		
//   GPIOB->ODR|=1<<8;//PB8 输出高电平
	
// }
