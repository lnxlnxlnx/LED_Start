/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-10     zhuqinsheng  the first version
 * 2025-12-17     zhuqinsheng  add ul_kprintf
 */
#include "ul_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"

#if ( ULOS_CONFIG_USE_KERNEL_PRINTF == 1 )
// 需要用户实现 ul_kwrite 函数
// example:
//int ul_kwrite(const char *buf, size_t len)
//{
//    HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 0xFFFF);
//    return len;
//}
ul_weak int ul_kwrite(const char *buf, size_t len)
{
    //有时使用这个函数会卡死，因为进入临界区后，无法进入uart的中断处理函数，导致消费者线程一直等待队列中的消息，最终系统卡死。
    size_t i;
    
    for (i = 0; i < len; i++)
    {
        USART1->DR = buf[i];
        while ((USART1->SR & 0X40) == 0);
    }
    
    return len;
    // size_t i;
    
    // for (i = 0; i < len; i++)
    // {
    //     // 等待上一字节发送完成
    //     while (!(USART1->SR & USART_SR_TC));
    //     // 发送当前字节
    //     USART1->DR = buf[i];
    // }
    
    // // 等待最后一个字节发送完成
    // while (!(USART1->SR & USART_SR_TC));

    // return 0;
}


static char g_kprintf_buf[100];

void ul_kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    // 直接格式化到缓冲区
    int len = vsnprintf(g_kprintf_buf, sizeof(g_kprintf_buf), fmt, args);
    
    // 一次性输出整个字符串
    ul_kwrite(g_kprintf_buf, len);
    
    va_end(args);
}
#else
void ul_kprintf(const char *fmt, ...)
{
    return;
}
#endif
void ul_assert_handler(const char *expr, const char *func, int line)
{
    volatile char fake = 0;
    // 输出到串口
    ul_kprintf("\n!!! ASSERTION FAILED !!!\n");
    ul_kprintf("File: %s\n", func);    // 函数名
    ul_kprintf("Line: %d\n", line);    // 行号
    ul_kprintf("Expr: %s\n", expr);    // 失败的表达式 ← #EX 提供
    
    while(1) 
    {
        fake = 0;
    }
}
