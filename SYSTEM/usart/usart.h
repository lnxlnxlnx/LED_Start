#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//STM32F103核心板例程
//库函数版本例程
/********** mcudev.taobao.com 出品  ********/

//////////////////////////////////////////////////////////////////////////////////	 
//STM32开发板
//串口1初始化		   

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		    //使能（1）/禁止（0）串口1接收
#define USART2_REC_LEN  		200
#define EN_USART2_RX 			1
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u8  USART2_RX_BUF[USART2_REC_LEN];
extern u16 USART2_RX_STA;
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound);
void uart2_send_byte(u8 data);
void uart2_send_buf(const u8 *buf, u16 len);
#endif


