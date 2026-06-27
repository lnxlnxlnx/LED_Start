/**
 * @file    main.c
 * @brief   主程序 — 多功能秒表系统
 *          考试要求: 数码管高精度秒表 + 按键控制 + 记次冻结 + LED指示 + 串口日志
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
#include "stopwatch.h"   /* 多功能秒表 */
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"

int main(void)
{
    Stm32_Clock_Init(9);    // 系统时钟设置 72MHz
    uart_init(115200);      // 串口初始化 (波特率 115200, 用于 printf + 秒表日志)

    /* ── 外设初始化 ── */
    LED_Init();              // 初始化 PC0~PC7 LED (低电平亮)
    LED_SMG_Init();          // ★ 数码管 GPIO 初始化 (必须先于定时器启动)
    My_KEY_Init();           // 按键 GPIO 初始化 (PC8,PC9,PD2,PA0)
    My_TIM3_Init(99, 72 - 1);   // TIM3 1ms 中断 (秒表计时 + 数码管扫描 + 按键消抖)
    //EXTIX_Init();               // 禁用: 秒表在 TIM3 ISR 中处理按键, EXTI 会冲突

    printf("NANO STM32 多功能秒表系统\r\n");
    printf("KEY1: 启动/暂停  KEY2: 清零(暂停时)  WK_UP: 记次\r\n");

    /* ── 秒表初始化 ── */
    Stopwatch_Init();        // 清零, 数码管显示 00-00-00

    printf("=== 就绪 ===\r\n");

    while (1)
    {
        /* 所有工作在 TIM3 ISR (1ms) 中完成: 按键/计时/LED/数码管 */
        delay_ms(20);
    }
}
