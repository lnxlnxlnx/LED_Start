#include "sys.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_exti.h"
#include "core_cm3.h"
#include <stm32f10x_flash.h>


//STM32F103核心板例程
//库函数版本例程
/********** mcudev.taobao.com 出品  ********/

//////////////////////////////////////////////////////////////////////////////////	 

//STM32开发板
//系统中断分组设置化		   

//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}

//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	   
void My_Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIGGER_MODE){
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	uint16_t portSource, pinSource;

	//1. 开AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	//转换GPIOx为GPIO端口源
	switch(GPIOx){
		case GPIO_A: portSource = GPIO_PortSourceGPIOA; break;
		case GPIO_B: portSource = GPIO_PortSourceGPIOB; break;
		case GPIO_C: portSource = GPIO_PortSourceGPIOC; break;
		case GPIO_D: portSource = GPIO_PortSourceGPIOD; break;
		case GPIO_E: portSource = GPIO_PortSourceGPIOE; break;
		case GPIO_F: portSource = GPIO_PortSourceGPIOF; break;
		case GPIO_G: portSource = GPIO_PortSourceGPIOG; break;
		default: return; // 错误的端口
	}
	pinSource = (uint16_t)(BITx);

	// 2. AFIO->EXTI映射
	GPIO_EXTILineConfig(portSource, pinSource);	// 引脚编号数字

	// 3. 配置EXTI模式
	switch(TRIGGER_MODE){
		case FTIR: // 下降沿触发
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
			break;
		case RTIR: // 上升沿触发
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
			break;
		default: // 错误的触发模式
			return;
	}

	EXTI_InitStructure.EXTI_Line = (uint16_t)(1 << BITx);
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

	// 4. 初始化EXTI
	EXTI_Init(&EXTI_InitStructure);
}


void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//得到中断寄存器组的编号
	EXTOFFSET=(BITx%4)*4; 
	RCC->APB2ENR|=0x01;//使能io复用时钟			 
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR|=1<<BITx;//  开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
}

//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 设置全局中断分组（原NVIC_PriorityGroupConfig）
	switch(NVIC_Group) {
		case 0: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); break; // 0位抢占优先级,4位响应优先级
		case 1: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); break; // 1位抢占优先级,3位响应优先级
		case 2: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); break; // 2位抢占优先级,2位响应优先级
		case 3: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); break; // 3位抢占优先级,1位响应优先级
		case 4: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); break; // 4位抢占优先级,0位响应优先级
		default: return; // 错误的分组
	}

    // 2. SysTick配置1ms中断（原函数自带逻辑保留）
    SysTick_Config(SystemCoreClock / 1000);

    // 3. 填充NVIC结构体，标准库自动处理ISER、IP寄存器
    NVIC_InitStruct.NVIC_IRQChannel = (IRQn_Type)NVIC_Channel;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = NVIC_SubPriority;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 等价 NVIC->ISER |= 1<<ch
    NVIC_Init(&NVIC_InitStruct);   	  				   
}

// 关闭单个 NVIC 中断通道（和 MY_NVIC_Init 反操作）
void _NVIC_DeInit(u8 NVIC_Channel)
{
    // 关闭中断使能
    NVIC->ICER[NVIC_Channel / 32] = 1 << (NVIC_Channel % 32);
    
    // 清除中断挂起标志
    NVIC->ICPR[NVIC_Channel / 32] = 1 << (NVIC_Channel % 32);
    
    // 清空优先级（可选，更干净）
    NVIC->IP[NVIC_Channel] = 0;
}

// 关闭指定 EXTI 中断线（比如 EXTI9_5 里的 EXTI8、EXTI9）
void My_NVIC_DeInit(u8 NVIC_Channel){
	NVIC_InitTypeDef NVIC_InitStructure;

	// 1. 禁用对应的 NVIC 中断通道
	NVIC_InitStructure.NVIC_IRQChannel = (IRQn_Type)NVIC_Channel;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	// 2. 清除中断标志位
	NVIC_ClearPendingIRQ((IRQn_Type)NVIC_Channel);
}

// ban的时候不是看NVIC，而是上面Ex初始化的是哪个引脚就ban哪个8、9、2、0引脚就对应8、9、2、0的BITx
void My_Ex_NVIC_DeInit(u8 BITx){
    EXTI_InitTypeDef EXTI_InitStruct;

    EXTI_InitStruct.EXTI_Line = (uint16_t)(1 << BITx);
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = DISABLE; // 关闭IMR屏蔽位
    EXTI_Init(&EXTI_InitStruct);

    // 清除EXTI挂起标志位
    EXTI_ClearITPendingBit((uint16_t)(1 << BITx));
}

void Ex_NVIC_DeInit(u8 BITx)
{
    // 关闭中断屏蔽
    EXTI->IMR &= ~(1 << BITx);
    
    // 清除触发方式
    EXTI->FTSR &= ~(1 << BITx);
    EXTI->RTSR &= ~(1 << BITx);
    
    // 清除中断挂起标志
    EXTI->PR |= 1 << BITx;
}

/*
// 关闭 KEY_UP → EXTI0
MY_NVIC_DeInit(EXTI0_IRQn);
Ex_NVIC_DeInit(0);

// 关闭 K2 → EXTI2
MY_NVIC_DeInit(EXTI2_IRQn);
Ex_NVIC_DeInit(2);

// 关闭 K0/K1 → EXTI9_5
MY_NVIC_DeInit(EXTI9_5_IRQn);
Ex_NVIC_DeInit(8);
Ex_NVIC_DeInit(9);
*/

//不能在这里执行所有外设复位!否则至少引起串口不工作.		    
//把所有时钟寄存器复位		  
void MYRCC_DeInit(void)
{
    RCC_DeInit(); // 标准库一键复位所有RCC寄存器，替代大量寄存器赋值

    // 配置向量表，和原逻辑不变
#ifdef  VECT_TAB_RAM
    MY_NVIC_SetVectorTable(0x20000000, 0x0);
#else
    MY_NVIC_SetVectorTable(0x08000000,0x0);
#endif
}
void _RCC_DeInit(void)
{	
 	RCC->APB1RSTR = 0x00000000;//复位结束			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR = 0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.	  
  	RCC->APB2ENR = 0x00000000; //外设时钟关闭.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //使能内部高速时钟HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //复位HSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //复位HSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //关闭所有中断		 
	//配置向量表				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(0x20000000, 0x0);
#else   
	MY_NVIC_SetVectorTable(0x08000000,0x0);
#endif
}
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断
__asm void INTX_DISABLE(void)
{
	CPSID I;		  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE I;		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//进入待机模式	  
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//使能SLEEPDEEP位 (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
 	PWR->CSR|=1<<8;          //设置WKUP用于唤醒
	PWR->CR|=1<<2;           //清除Wake-up 标志
	PWR->CR|=1<<1;           //PDDS置位		  
	WFI_SET();				 //执行WFI指令		 
}	     
//系统软复位   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 		 
//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;	   
//#define JTAG_SWD_DISABLE   0X02
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_ENABLE    0X00		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
} 
//系统时钟初始化函数
//pll:选择的倍频数，从2开始，最大值为16		 

/*
复位 RCC
开启 HSE
先配置 FLASH 访问延时
配置 AHB/APB 分频
配置 PLL、开启 PLL
切换系统时钟到 PLL
*/
void Stm32_Clock_Init(u8 PLL)
{
    unsigned char temp=0;
    MYRCC_DeInit();

    // 开启HSE
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_WaitForHSEStartUp() != SUCCESS); // 等待HSE稳定

    // FLASH延时2周期
	RCC_AHBPeriphClockCmd(RCC_AHBENR_FLITFEN | RCC_AHBENR_SRAMEN, ENABLE); // 使能FLASH和SRAM时钟
    FLASH_SetLatency(FLASH_Latency_2);

    // AHB、APB预分频
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    // PLL配置 HSE倍频
    //PLL -= 2;
    //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, (RCC_PLLMul_TypeDef)PLL);
	switch(PLL) {
		case 2:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2); break;
		case 3:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3); break;
		case 4:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4); break;
		case 5:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_5); break;
		case 6:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6); break;
		case 7:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_7); break;
		case 8:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_8); break;
		case 9:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); break;
		case 10:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_10); break;
		case 11:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_11); break;
		case 12:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12); break;
		case 13:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_13); break;
		case 14:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_14); break;
		case 15:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_15); break;
		case 16:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_16); break;
		default:RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); break;
	}
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL锁定

    // 切换系统时钟为PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(1)
    {
        temp = RCC_GetSYSCLKSource();       // 返回 CFGR[3:2] = 0x00/0x04/0x08
        if(temp == RCC_CFGR_SWS_PLL) break; // 0x08 = PLL 已就绪
    }
}

void _Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //复位并配置向量表
 	RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	while(!(RCC->CR>>17));//等待外部时钟就绪
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//抵消2个单位
	RCC->CFGR|=PLL<<18;   //设置PLL值 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2个延时周期

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟	 
	while(temp!=0x02)     //等待PLL作为系统时钟设置成功
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}    
}		    

//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量			 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 	   	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}
