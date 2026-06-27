#include "dma.h"
#include "delay.h"	
#include "stm32f10x_dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//DMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量  

void MY_DMA_Config(DMA_Channel_TypeDef* DMA_CHx, u32 peripheralBaseAddr, u32 bufferBaseAddr, u16 bufferSize){
	// 1. 开启 DMA1 时钟
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	delay_ms(5); // 等待 DMA 时钟稳定

	//2. 配置 DMA1 初始化结构体
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_BufferSize = bufferSize;
	DMA_InitStructure.DMA_MemoryBaseAddr = bufferBaseAddr;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	// 从内存到外设(外设作为目标（Destination）)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_PeripheralBaseAddr = peripheralBaseAddr;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	// 外设数据宽度为8位
	DMA_InitStructure.DMA_PeripheralInc = DMA_MemoryInc_Disable;	// 外设地址不增量
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	// 设置优先级为中等

	//3. 初始化
	DMA_Init(DMA_CHx, &DMA_InitStructure);
	DMA1_MEM_LEN = bufferSize; // 保存 DMA 传输数据量

	//4. 先关闭 DMA 传输，初始化时不能打开 DMA
	DMA_Cmd(DMA_CHx, DISABLE);
}
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx, u32 cpar, u32 cmar, u16 cndtr)
{
	RCC->AHBENR |= 1 << 0;			//开启DMA1时钟
	delay_ms(5);				//等待DMA时钟稳定
	DMA_CHx->CPAR = cpar; 	 	//DMA1 外设地址 
	DMA_CHx->CMAR = (u32)cmar; 	//DMA1,存储器地址
	DMA1_MEM_LEN = cndtr;      	//保存DMA传输数据量
	DMA_CHx->CNDTR = cndtr;    	//DMA1,传输数据量
	DMA_CHx->CCR = 0X00000000;	//复位
	DMA_CHx->CCR |= 1 << 4;  		//从存储器读
	DMA_CHx->CCR |= 0 << 5;  		//普通模式
	DMA_CHx->CCR |= 0 << 6; 		//外设地址非增量模式
	DMA_CHx->CCR |= 1 << 7; 	 	//存储器增量模式
	DMA_CHx->CCR |= 0 << 8; 	 	//外设数据宽度为8位
	DMA_CHx->CCR |= 0 << 10; 		//存储器数据宽度8位
	DMA_CHx->CCR |= 1 << 12; 		//中等优先级
	DMA_CHx->CCR |= 0 << 14; 		//非存储器到存储器模式		  	
}
//开启一次DMA传输

//DMA 初始化函数
void dma_init(u32 cmar, u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	delay_ms(5); // 等待 DMA 时钟稳定
	/*DMA配置*/

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//串口数据寄存器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cmar; //内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //方向(从内存到外设)
	DMA_InitStructure.DMA_BufferSize = cndtr; //传输内容的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //内存数据单位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA模式：一次传输，循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //禁止内存到内存的传输

    // 3. 初始化 DMA
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	DMA1_MEM_LEN = cndtr; // 保存 DMA 传输数据量

    // 4. 初始化时不能打开 DMA！！！
    DMA_Cmd(DMA1_Channel4, DISABLE);
}

void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx)
{
	DMA_CHx->CCR &= ~(1 << 0);       //关闭DMA传输 
	DMA_CHx->CNDTR = DMA1_MEM_LEN; //DMA1,传输数据量 
	DMA_CHx->CCR |= 1 << 0;          //开启DMA传输
}
void MY_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx){
	// 1. 关闭DMA
	DMA_Cmd(DMA_CHx, DISABLE);

	// 2. 设置传输长度
	DMA_SetCurrDataCounter(DMA_CHx, DMA1_MEM_LEN);

	// 3. dma_init
	DMA_Cmd(DMA_CHx, ENABLE);
}
