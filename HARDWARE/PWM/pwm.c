#include "pwm.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//PWM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//TIM3 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  

	RCC->APB1ENR|=1<<1;	//TIM3时钟使能 
	RCC->APB2ENR|=1<<4; //使能PORTC时钟	 
	//当定时器需要输出信号到引脚时（如 PWM）使能端口
	//定时器是独立外设，不依赖 GPIO
	GPIOC->CRL&=0XF0FFFFFF;	//PC6输出
	GPIOC->CRL|=0X0B000000;	//复用功能输出 	  
	RCC->APB2ENR|=1<<0;     //使能AFIO时钟	
    AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]
	AFIO->MAPR|=3<<10;      //部分重映像,TIM3_CH1->PC6
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频
	TIM3->CCMR1|=7<<4;  	//CH1 PWM2模式		 
	TIM3->CCMR1|=1<<3; 	    //CH1预装载使能	   
	TIM3->CCER|=1<<0;   	//OC1 输出使能	   
	TIM3->CR1=0x0080;   	//ARPE使能 
	TIM3->CR1|=0x01;    	//使能定时器3 	
		
}

