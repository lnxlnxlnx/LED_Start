#include "pwm.h"
#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//PWM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//TIM3 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  

	RCC->APB1ENR|=1<<1;	//TIM3时钟使能 
	RCC->APB2ENR|=1<<4; //使能PORTC时钟	 
	//当定时器需要输出信号到引脚时（如 PWM）使能端口
	//定时器是独立外设，不依赖 GPIO
	GPIOC->CRL&=0XF0FFFFFF;	//PC6输出
	GPIOC->CRL|=0X0B000000;	//复用功能输出 	  
	RCC->APB2ENR|=1<<0;     //使能AFIO时钟	
    AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]
	AFIO->MAPR|=3<<10;      //部分重映像,TIM3_CH1->PC6
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频
	TIM3->CCMR1|=7<<4;  	//CH1 PWM2模式		 
	TIM3->CCMR1|=1<<3; 	    //CH1预装载使能	   
	TIM3->CCER|=1<<0;   	//OC1 输出使能	   
	TIM3->CR1=0x0080;   	//ARPE使能 
	TIM3->CR1|=0x01;    	//使能定时器3 	
		
}


/****************************************************************
函数功能：TIM3 PWM初始化（PC6=CH1，PC7=CH2，完全重映射）
入口参数：arr：自动重装载值
          psc：预分频器
备注：频率 = 72M / ((arr+1)*(psc+1))
TIM3_CH1: 默认 PA6, 部分重映射 -> PC6
TIM3_CH2: 默认 PA7, 部分重映射 -> PC7
TIM3_CH3: 默认 PB0, 部分重映射 -> PB0 (无变化)
TIM3_CH4: 默认 PB1, 部分重映射 -> PB1 (无变化)
*****************************************************************/
void TIM3_PWM_Init_2(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 1. 开时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    // TIM3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    // 2. GPIO配置 PC6 PC7 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 复用推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. 完全重映射 TIM3_CH1->PC6 CH2->PC7
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    // 4. 定时器基础配置
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 5. PWM模式配置 CH1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM 模式 1，当 CNT < CCRx 时输出有效电平，否则输出无效电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 由于LED是共阳的，所以设置为低电平有效
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比为0%，但是没什么用，因为我们后面会通过 SetCompare 函数来设置占空比 
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    // CH2 同配置
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    // 6. 使能预装载
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // 7. 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

/****************************************************************
函数功能：设置TIM3 CH1(PC6)和CH2(PC7)的占空比
入口参数：duty1：PC6占空比 0~100
          duty2：PC7占空比 0~100
备注：占空比 = (CCR / ARR) * 100%，80%就是80%的时间亮，这是因为led本身是共阳的，所以高电平是灭，低电平是亮
*****************************************************************/
void TIM3_Set_PWM_Duty(u8 duty1, u8 duty2)
{
    u16 ccr1, ccr2;
    u16 arr = TIM3->ARR;  // 获取自动重装载值

    // 限幅 0~100
    if(duty1 > 100) duty1 = 100;
    if(duty2 > 100) duty2 = 100;

    ccr1 = (u16)(arr * duty1 / 100.0);
    ccr2 = (u16)(arr * duty2 / 100.0);

    TIM_SetCompare1(TIM3, ccr1);
    TIM_SetCompare2(TIM3, ccr2);
}