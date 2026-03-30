#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//通用定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void TIM3_Init(u16 arr,u16 psc); 
void TIM6_Init(u16 arr,u16 psc); //有1~8个定时器，TIM6是基本定时器，不能产生PWM波，只有更新事件，可以用来做系统滴答时钟
void TIM7_Init(u16 arr,u16 psc); //有1~8个定时器，TIM7是基本定时器，不能产生PWM波，只有更新事件，可以用来做系统滴答时钟

void TIM2_Cap_Init(u16 arr,u16 psc);
 
#endif
