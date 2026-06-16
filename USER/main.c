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
    My_KEY_Init();				//按键初始化
    //MYDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)SendBuff, (TEXT_LENTH) * 100); //DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH)*100.
    dma_init((u32)SendBuff, (TEXT_LENTH) * 100);
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
        current_key = KEY_Scan(USE_LONGPRESS);

        switch (current_key)
        {
        case KEY0_PRES:
            LED0 = !LED0; // 按下 KEY0 时点亮 LED0
            break;
        case KEY1_PRES:
            LED1 = !LED1; // 按下 KEY1 时点亮 LED1
            break;
        case KEY2_PRES:
            LED2 = !LED2; // 按下 KEY2 时点亮 LED2
            break;
        case WKUP_PRES:
            LED3 = !LED3; // 按下 WK_UP 时点亮 LED3
        default:
            break;
        }
        delay_ms(200);
    }
}
