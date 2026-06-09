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

const u8 TEXT_ARR[] = { "实验12 直接存储访问（DMA）实验\r\n" }; //sizeof(u8) = 1，所以 sizeof(TEXT_ARR) 就是字符串长度（包含结束符），因此需要减去 1 来得到不包含结束符的长度
#define TEXT_LENTH  sizeof(TEXT_ARR)-1			//TEXT_ARR字符串长度(不包含结束符)
u8 SendBuff[(TEXT_LENTH) * 100];

int main(void)
{
    Stm32_Clock_Init(9);    //系统时钟设置
    uart_init(115200);  //串口初始化
    LED_Init();		  		//初始化与LED连接的硬件接口
    KEY_Init();				//按键初始化
    MYDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)SendBuff, (TEXT_LENTH + 2) * 100); //DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
    printf("NANO STM32\r\n");
    printf("DMA TEST\r\n");
    printf("KEY0:Start\r\n");

    //显示提示信息
    u8 t = 0;
    for (int i = 0; i < (TEXT_LENTH) * 100; i++) //填充ASCII字符集数据
    {
        SendBuff[i] = TEXT_ARR[t++]; //复制TEXT_ARR语句
        if (t >= TEXT_LENTH) //加入换行符
        {
            t = 0;
        }
    }
    u8 i = 0;
    u8 current_key = 0;
    while (1)
    {
        current_key = KEY_Scan(0);

        if (current_key == KEY0_PRES)  // KEY0 按下
        {
            printf("\r\nDMA DATA:\r\n ");

            // 1. 使能串口1 DMA发送（寄存器写法 USART1->CR3 |= 1<<7; 的标准库）
            USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

            DMA_SetCurrDataCounter(DMA1_Channel4, sizeof(SendBuff)); // 设置 DMA 传输数据量

            // 2. 使能DMA通道，开始一次传输（MYDMA_Enable(DMA1_Channel4)）
            DMA_Cmd(DMA1_Channel4, ENABLE);

            // 等待DMA传输完成，同时LED2闪烁
            while (1)
            {
                // 判断 DMA1 通道4 传输完成标志
                if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
                {
                    DMA_ClearFlag(DMA1_FLAG_TC4); // 清除标志
                    break;
                }

                LED7 = !LED7;
                delay_ms(50);
            }

            LED7 = 1;
            printf("Transimit Finished!\r\n");
            // 2. 传输完成后关闭DMA
            DMA_Cmd(DMA1_Channel4, DISABLE);
        }

        i++;
        delay_ms(10);

        if (i == 20)
        {
            LED0 = !LED0;
            i = 0;
        }
    }
}
