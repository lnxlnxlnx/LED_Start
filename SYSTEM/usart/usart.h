#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "stm32f10x_usart.h"

//STM32F103核心板例程
//库函数版本例程
/********** mcudev.taobao.com 出品  ********/

//////////////////////////////////////////////////////////////////////////////////	 
//STM32开发板
//串口1初始化		   

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		    //使能（1）/禁止（0）串口1接收
	  	
extern volatile u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern volatile u16 USART_RX_STA;         		//接收状态标记	

/* ── 简单环形接收缓冲 (绕过 USART_RX_STA 协议) ── */
#define RX_RING_LEN  256
extern volatile u8  g_rx_ring[RX_RING_LEN];
extern volatile u16 g_rx_ring_wr;       // 写入位置 (ISR 中递增)
extern volatile u16 g_rx_ring_rd;       // 读取位置 (主循环中递增)
extern volatile u8  g_rx_ring_has_data; // 是否有新数据

void uart_init(u32 bound);
#endif


