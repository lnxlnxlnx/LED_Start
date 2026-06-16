/**
 * @file    main.c
 * @brief   主程序
 *          每 1 秒在 8 位数码管上循环显示 0~9
 */

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key_led_one.h"
#include "project_log_config.h"
#include "rtc.h"
#include "dac.h"
#include "timer.h"
#include "remote.h"
#include "smg.h"
#include "pwm.h"
#include "adc.h"
#include "dma.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"

int main(void)
{
    Stm32_Clock_Init(9);    //系统时钟设置
    uart_init(115200);  //串口初始化
    LED_Init();		  		//初始化与LED连接的硬件接口
    My_KEY_Init();				//按键初始化
    My_TIM3_Init(999, 72 - 1);				//定时器初始化
    EXTIX_Init();                //外部中断初始化
    printf("NANO STM32\r\n");
    printf("DMA TEST\r\n");
    printf("KEY0:Start\r\n");
    TIM3_PWM_Init(999, 72 - 1); // 初始化 TIM3 用于 PWM 输出，频率约为 1 kHz

    u8 i = 0;
    u8 current_key = 0;
    while (1)
    {
        delay_ms(200);
    }
}
