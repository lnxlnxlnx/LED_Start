#include "exti.h"
#include "led.h"
//#include "beep.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "key_led_one.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  	  
 
 
//外部中断初始化函数
void EXTIX_Init(void)
{
 
    KEY_Init();//初始化按键对应io模式
    Ex_NVIC_Config(GPIO_C,8,FTIR); 		//下降沿触发
	Ex_NVIC_Config(GPIO_C,9,FTIR);		//下降沿触发
	Ex_NVIC_Config(GPIO_D,2,FTIR);		//下降沿触发
	Ex_NVIC_Config(GPIO_A,0,RTIR);		//上升沿触发

	MY_NVIC_Init(2,2,EXTI0_IRQn,2);    	//抢占2，子优先级2，组2
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//抢占2，子优先级1，组2
	MY_NVIC_Init(2,0,EXTI2_IRQn,2);  	//抢占2，子优先级0，组2	  
 
} 
extern EVENT_TYPE curent_event;
//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
    delay_ms(10);    //消抖
	// if(WK_UP==1)     //KEY_UP按键
	// {	  
	// 	LED0=!LED0;
	// 	LED1=!LED1;	
	// }
	//curent_event = kUP; // 设置当前事件为kUP
	if (curent_event == KNONE) {
    	curent_event = kUP;
	}
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位
}
//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
    delay_ms(10);    //消抖			 
    // if(KEY2==0)	     //KEY2按键
    // {
    //    BEEP=!BEEP;
    // }
	curent_event = K2;
    EXTI->PR=1<<2;     //清除LINE2上的中断标志位  
}
//外部中断5_9服务中断程序
 void EXTI9_5_IRQHandler(void)
{		
	delay_ms(10);   //消抖			 
	if(KEY0==0)	    //KEY0按键
	{
		//LED0=!LED0;
		curent_event = K0;
	}
 	if(KEY1==0)	    //KEY1按键
	{
		//LED1=!LED1;
		curent_event = K1;
	}
	EXTI->PR=1<<8;  //清除LINE8上的中断标志位  
	EXTI->PR=1<<9;  //清除LINE9上的中断标志位  	
}
