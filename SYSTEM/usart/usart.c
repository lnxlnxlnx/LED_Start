#include "sys.h"
#include "usart.h"

// STM32F103核心板例程
// 库函数版本例程
/********** mcudev.taobao.com 出品  ********/

//////////////////////////////////////////////////////////////////////////////////
// 如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h" //ucos 使用
#endif
//////////////////////////////////////////////////////////////////////////////////
// STM32开发板
// 串口1初始化

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕
	USART1->DR = (u8)ch;
	return ch;
}
#endif

/*使用microLib的方法*/
/*
int fputc(int ch, FILE *f)
{
   USART_SendData(USART1, (uint8_t) ch);

   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
}
*/

#if EN_USART1_RX // 如果使能了接收
// 串口1中断服务程序
// 注意,读取USARTx->SR能避免莫名其妙的错误
volatile u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.
// 接收状态
// bit15，	接收完成标志
// bit14，	接收到0x0d
// bit13~0，	接收到的有效字节数目
volatile u16 USART_RX_STA = 0; // 接收状态标记，这个变量是一个协议状态变量，最高位表示接收完成标志，次高位表示接收到0x0d，低13位表示接收到的有效字节数目

void uart_init(u32 bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟
																				  // USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Usart1 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  // 先统一中断分组，避免后续重配置导致优先级错乱

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0 (与TIM3同级，不抢占)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0 (同组内最高，同时挂起时先执行)
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器

	// USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;										// 一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式

	USART_Init(USART1, &USART_InitStructure);	   // 初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启中断
	USART_Cmd(USART1, ENABLE);					   // 使能串口

	/* ── 清除上电/初始化期间可能产生的 USART 错误标志 ── */
	(void)USART1->SR;           // 读 SR 清除部分标志
	(void)USART1->DR;           // 读 DR 清除 RXNE + ORE
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC // 如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();
#endif

	/* ── ORE 溢出错误: 不处理会导致 RXNE 永久阻塞 ── */
	if (USART1->SR & (1 << 3))          // ORE=1 (bit3)
	{
		(void)USART1->SR;               // 读 SR : ORE 清零序列首步
		(void)USART1->DR;               // 读 DR : 完成 ORE 清零
	}
	/* ── 其他错误标志 (FE/NE/PE) 不清除也不影响 RXNE ── */

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 接收中断
	{
		Res = USART_ReceiveData(USART1);

		if ((USART_RX_STA & 0x8000) == 0) // 接收未完成
		{
			if (USART_RX_STA & 0x4000) // 接收到了0x0d (\r)
			{
				if (Res == 0x0a)         // \n 补全 \r\n
					USART_RX_STA |= 0x8000;
				else                     // 其他字符 → 错误, 重新开始
					USART_RX_STA = 0;
			}
			else // 还没收到 \r
			{
				if (Res == 0x0d)              // 收到 \r, 等待 \n
					USART_RX_STA |= 0x4000;
				else if (Res == 0x0a)         // 单独 \n → 兼容纯 \n 终端
					USART_RX_STA |= 0x8000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0;
				}
			}
		}
		printf("%c", Res); // 回显
	}
#ifdef OS_TICKS_PER_SEC // 如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();
#endif
}
#endif
