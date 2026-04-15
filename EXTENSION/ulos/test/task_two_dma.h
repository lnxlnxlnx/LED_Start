#ifndef D5B2632E_9A68_4280_A037_DE8D163B7055
#define D5B2632E_9A68_4280_A037_DE8D163B7055
#include "ul_thread.h"
#include "led.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "dma.h"

const u8 TEXT_TO_SEND[] = {"ALIENTEK NANO STM32 DMA 串口实验"};
#define TEXT_LENTH  sizeof(TEXT_TO_SEND)-1			//TEXT_TO_SEND字符串长度(不包含结束符)
u8 SendBuff[(TEXT_LENTH + 2) * 100];

void thread_dma_entry(void *p)
{
    static u16 i;
    static u8 t = 0;
    KEY_Init();  // 按键初始化
    MYDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)SendBuff, (TEXT_LENTH + 2) * 100); // DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
    printf("NANO STM32\r\n");
    printf("DMA TEST\r\n");
    printf("KEY0:Start\r\n");

    // 显示提示信息
    for (i = 0; i < (TEXT_LENTH + 2) * 100; i++) // 填充ASCII字符集数据
    {
        if (t >= TEXT_LENTH) // 加入换行符
        {
            SendBuff[i++] = 0x0d;
            SendBuff[i] = 0x0a;
            t = 0;
        }
        else
            SendBuff[i] = TEXT_TO_SEND[t++]; // 复制TEXT_TO_SEND语句
    }

    i = 0;

    while (1)
    {
        t = KEY_Scan(0);

        if (t == KEY0_PRES) // KEY0按下
        {
            printf("\r\nDMA DATA:\r\n ");
            USART1->CR3 = 1 << 7;        // 使能串口1的DMA发送
            MYDMA_Enable(DMA1_Channel4); // 开始一次DMA传输！

            // 等待DMA传输完成，此时我们来做另外一些事，点灯
            // 实际应用中，传输数据期间，可以执行另外的任务
            while (1)
            {
                if (DMA1->ISR & (1 << 13)) // 等待通道4传输完成
                {
                    DMA1->IFCR |= 1 << 13; // 清除通道4传输完成标志
                    break;
                }

                LED2 = !LED2;
                ul_thread_delay(50);
            }

            LED2 = 1;
            printf("Transimit Finished!\r\n"); // 提示传送完成
        }

        i++;
        ul_thread_delay(10);

        if (i == 20)
        {
            LED0 = !LED0; // 提示系统正在运行
            i = 0;
        }
    }
}

void dma_example(void)
{
    ul_thread_t *thread_dma_p = ul_thread_create("thread_dma", thread_dma_entry, UL_NULL, 512, 0, 1);
    ul_thread_startup(thread_dma_p);
}

#endif /* D5B2632E_9A68_4280_A037_DE8D163B7055 */
