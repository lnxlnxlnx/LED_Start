#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "dma.h"

/************************************************
 ALIENTEK NANO STM32开发板实验17
 DMA实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/

const u8 TEXT_TO_SEND[] = {"ALIENTEK NANO STM32 DMA 串口实验"};
#define TEXT_LENTH  sizeof(TEXT_TO_SEND)-1			//TEXT_TO_SEND字符串长度(不包含结束符)
u8 SendBuff[(TEXT_LENTH + 2) * 100];

int main(void)
{
    u16 i;
    u8 t = 0;

    Stm32_Clock_Init(9);    //系统时钟设置
    uart_init(115200);  //串口初始化
    LED_Init();		  		//初始化与LED连接的硬件接口
    KEY_Init();				//按键初始化
    //MYDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)SendBuff, (TEXT_LENTH + 2) * 100); //DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
    dma_init((u32)SendBuff, (TEXT_LENTH + 2) * 100); //DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
    //MY_DMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)SendBuff, (TEXT_LENTH + 2) * 100); //DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
    printf("NANO STM32\r\n");
    printf("DMA TEST\r\n");
    printf("KEY0:Start\r\n");

    //显示提示信息
    for (i = 0; i < (TEXT_LENTH + 2) * 100; i++) //填充ASCII字符集数据
    {
        if (t >= TEXT_LENTH) //加入换行符
        {
            SendBuff[i++] = 0x0d;
            SendBuff[i] = 0x0a;
            t = 0;
        }
        else SendBuff[i] = TEXT_TO_SEND[t++]; //复制TEXT_TO_SEND语句
    }

    i = 0;

    while (1)
    {
        t = KEY_Scan(0);

        if (t == KEY0_PRES) //KEY0按下
        {
            printf("\r\nDMA DATA:\r\n ");
            USART1->CR3 = 1 << 7;       //使能串口1的DMA发送
            MYDMA_Enable(DMA1_Channel4);//开始一次DMA传输！

            //等待DMA传输完成，此时我们来做另外一些事，点灯
            //实际应用中，传输数据期间，可以执行另外的任务
            while (1)
            {
                if (DMA1->ISR & (1 << 13)) //等待通道4传输完成
                {
                    DMA1->IFCR |= 1 << 13; //清除通道4传输完成标志
                    break;
                }

                LED2 = !LED2;
                delay_ms(50);
            }

            LED2 = 1;
            printf("Transimit Finished!\r\n");//提示传送完成
        }

        i++;
        delay_ms(10);

        if (i == 20)
        {
            LED0 = !LED0; //提示系统正在运行
            i = 0;
        }
    }

}




