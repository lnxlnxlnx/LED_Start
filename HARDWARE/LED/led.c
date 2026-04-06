#include "led.h"
#include "delay.h"
#include "project_log_config.h"
#if !defined(LOG_TAG)
    #define LOG_TAG                    "LED"
#endif
#undef LOG_LVL
#if defined(LED_LOG_LVL)
    #define LOG_LVL                    LED_LOG_LVL
#endif
#include <elog.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // 使能 GPIOC 时钟

    // 配置 PC0~PC7 为 推挽输出、50MHz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 速度50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);               // 初始化 GPIOC

    // PC0~PC7 全部输出高电平（和你原来的 ODR 效果一致）
    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                   GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
                   log_e("LED_Init", "LED_Init OK");
                   log_w("LED_Init", "LED_Init OK");
                   log_i("LED_Init", "LED_Init OK");
}
void led0_operate(uint8_t val) { LED0 = val; }  //DEPRECATED:其实可以用宏拼接?
void led1_operate(uint8_t val) { LED1 = val; }
void led2_operate(uint8_t val) { LED2 = val; }
void led3_operate(uint8_t val) { LED3 = val; }
void led4_operate(uint8_t val) { LED4 = val; }
void led5_operate(uint8_t val) { LED5 = val; }
void led6_operate(uint8_t val) { LED6 = val; }
void led7_operate(uint8_t val) { LED7 = val; }

LED_Operate led_funcs[] = {//DEPRECATED:其实可以用宏拼接?
    led0_operate, led1_operate, led2_operate, led3_operate,
    led4_operate, led5_operate, led6_operate, led7_operate
};
/* 测试用循环控制函数 */
void led_loop_control(void)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        led_funcs[i](0);
        delay_ms(200);
        led_funcs[i](1);
    }
    for (i = 0; i < 8; i++)
        led_funcs[i](0);
    delay_ms(1000);
    for (i = 0; i < 8; i++)
        led_funcs[i](1);
}
 

