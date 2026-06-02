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

int main(void)
{
    Stm32_Clock_Init(9);         // 系统时钟 72MHz
    LED_Init();
    NVIC_Configuration();        // 中断分组 2
    uart_init(115200);           // 串口 115200bps
    LED_SMG_Init();
    Adc_Init();                  // 初始化 ADC，定时器触发转换

    TIM4_Init(9, 7199);          // TIM4 1ms 中断，驱动数码管扫描 + 自动轮播
    TIM3_Init(9, 7199);          // TIM3 1ms 中断，驱动数码管扫描 + 自动轮播


    while (1);                   // 所有逻辑都在 TIM4 中断中完成
}
