#ifndef CA8F9A09_5562_4B48_A36C_EF4FB6F84194
#define CA8F9A09_5562_4B48_A36C_EF4FB6F84194
#include "led.h"
#include "ul_thread.h"
#include "usart_helper.h"

// 定义串口实例
static USART_Helper_Instance_t g_usart1 = {
    .instance = USART1,
    .rx_sta = &USART_RX_STA,
    .rx_buf = USART_RX_BUF,
    .callback = USART_Callback_Print,
    .enabled = 0
};

// 自定义回调示例：收到命令后执行特定操作
void My_Custom_Callback(u8 *buf, u8 len)
{
    printf("\r\n[自定义处理] 收到 %d 字节:\r\n", len);
    USART_Helper_SendBuf(&g_usart1, buf, len);
    printf("\r\n");
    
    // 这里可以添加命令解析逻辑
    if (len >= 4 && buf[0] == 'L' && buf[1] == 'E' && buf[2] == 'D' && buf[3] == '1')
    {
        LED1 = !LED1;
        printf("LED1 状态切换!\r\n");
    }
}

void thread_uart_entry(void *p)
{
    u16 times = 0;
    
    // uart_init 在 main 中已调用，这里不需要重复
    USART_Helper_Init(); // 初始化串口助手模块
    USART_Helper_Register(&g_usart1); // 注册 USART1 实例
    // ✅ 使用封装的处理函数
    printf("\r\n串口助手初始化完成，请输入数据，以回车键结束\r\n");
    while (1)
    {
        USART_Helper_Process(&g_usart1);  // 调用处理函数
        
        times++;
        if (times % 5000 == 0)
        {
            printf("\r\nALIENTEK NANO STM32开发板 串口实验\r\n");
            printf("正点原子@ALIENTEK\r\n\r\n\r\n");
        }
        if (times % 200 == 0)
            printf("请输入数据，以回车键结束\r\n");
        if (times % 30 == 0)
            LED7 = !LED7;
        
        ul_thread_delay(10);
    }
}

void thread_uart_test_entry(void *p)
{
    u8 t;
    u8 len;
    u16 times = 0;
    
    //uart_init(72, 115200);
    
    while (1)
    {
        if (USART_RX_STA & 0x8000)  // 原始判断
        {
            len = USART_RX_STA & 0x3fff;
            printf("\r\n您发送的消息为:\r\n\r\n");
            
            for (t = 0; t < len; t++)
            {
                USART1->DR = USART_RX_BUF[t];
                while ((USART1->SR & 0X40) == 0);
            }
            
            printf("\r\n\r\n");
            USART_RX_STA = 0;
        }
        // ...
    }
}

#endif /* CA8F9A09_5562_4B48_A36C_EF4FB6F84194 */
