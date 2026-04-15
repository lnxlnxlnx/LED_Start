#ifndef B6CABA67_16FD_43C0_8574_149796EA8767
#define B6CABA67_16FD_43C0_8574_149796EA8767
#include "ul_thread.h"
#include "led.h"
//#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
void thread0_entry(void *p)
{
    while(1)
    {
        //LED_Turn(1);
        LED0 = !LED0;
        ul_thread_delay(100);
    }
}

void thread1_entry(void *p)
{
    while(1)
    {
        //LED_Turn(2);
        LED1 = !LED1;
        ul_thread_delay(500);
    }
}

void thread2_entry(void *p)
{
    while(1)
    {
        //LED_Turn(3);
        LED2 = !LED2;
        ul_thread_delay(1000);
        //delay_ms(1000); //测试使用系统滴答时钟的延时函数，看看是否会影响系统滴答时钟的正常工作。
        //用了后慢了一倍
    }
}

void thread7_pwm_entry(void *p)
{
    u16 led6pwmval = 0;
    u8 dir = 1;
    TIM3_PWM_Init(899, 0); //不分频。PWM频率=72000/(899+1)=80Khz
    while (1)
    {
        ul_thread_delay(10);

        if (dir)led6pwmval++;
        else led6pwmval--;

        if (led6pwmval > 300)dir = 0;

        if (led6pwmval == 0)dir = 1;

        LED6_PWM_VAL = led6pwmval;
    } 
}

void thread_uart_entry(void *p)
{
    u8 t;
    u8 len;
    u16 times = 0;
    uart_init(72, 115200); // 初始化串口，波特率115200
    while (1)
    {
        if (USART_RX_STA & 0x8000)
        {
            len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
            printf("\r\n您发送的消息为:\r\n\r\n");

            for (t = 0; t < len; t++)
            {
                USART1->DR = USART_RX_BUF[t];

                while ((USART1->SR & 0X40) == 0); //等待发送结束
            }

            printf("\r\n\r\n");//插入换行
            USART_RX_STA = 0;
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\r\nALIENTEK NANO STM32开发板 串口实验\r\n");
                printf("正点原子@ALIENTEK\r\n\r\n\r\n");
            }

            if (times % 200 == 0)printf("请输入数据,以回车键结束\r\n");

            if (times % 30 == 0)LED7 = !LED7; //闪烁LED,提示系统正在运行.

            ul_thread_delay(10);
        }
    }
}
void start_thread_entry(void *p)
{
    ul_thread_t *tid0 = ul_thread_create("t0", thread0_entry, UL_NULL, 512, 0, 1);
    ul_thread_t *tid1 = ul_thread_create("t1", thread1_entry, UL_NULL, 512, 0, 1);
    ul_thread_t *tid2 = ul_thread_create("t2", thread2_entry, UL_NULL, 512, 0, 1);
    ul_thread_t *tid7_pwm = ul_thread_create("t7_pwm", thread7_pwm_entry, UL_NULL, 512, 0, 1);
    ul_thread_t *tid_uart = ul_thread_create("t_uart", thread_uart_entry, UL_NULL, 512, 0, 1);
    ul_thread_startup(tid0);
    ul_thread_startup(tid1);
    ul_thread_startup(tid2);
    ul_thread_startup(tid7_pwm);
    ul_thread_startup(tid_uart);
    while(1)
    {
        ul_thread_delay(10);
    }
}

#endif /* B6CABA67_16FD_43C0_8574_149796EA8767 */
