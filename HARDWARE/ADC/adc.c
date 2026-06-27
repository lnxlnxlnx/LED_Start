#include "adc.h"
#include "delay.h"
#include "smg.h"
#include "led.h"
#include "timer.h"
#include "stm32f10x_adc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	      
//初始化ADC	
//这里我们仅以规则通道为例
//我们默认仅开启通道9	

void My_ADC_Init(void)
{
	//Adc_Init(); // ADC初始化
	// 这里可以添加定时器触发ADC转换的配置
	// 例如使用TIM3每秒触发一次ADC转换
	//TIM3_Init(7199, 9999); // 1Hz触发频率
	// 1. 开启时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE); // 使能GPIOB和ADC1时钟
	
	// 2.// 配置PB9为模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 3. 配置预分频器和采样时间
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 单次转换模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1; // 仅转换一个通道
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
	ADC_Init(ADC1, &ADC_InitStructure);

	// 4. 配置通道9的采样时间
	ADC_RegularChannelConfig(ADC1, ADC_CH9, 1, ADC_SampleTime_239Cycles5); // 通道9，采样时间239.5周期

	// 5. 开启ADC
	ADC_Cmd(ADC1, ENABLE);

	// 6. 校准ADC
	ADC_ResetCalibration(ADC1); // 复位校准
	while (ADC_GetResetCalibrationStatus(ADC1)); // 等待复位校准完成

	ADC_StartCalibration(ADC1); // 开始校准
	while (ADC_GetCalibrationStatus(ADC1)); // 等待校准完成
}
void  Adc_Init(void)
{ 	
	//先初始化IO口
 	RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 
	GPIOB->CRH&=0XFFFFFF0F;//PB9 anolog输入
	//通道设置			 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发->定时器每200ms触发一次ADC转换
	ADC1->CR2&=~(1<<11);   //右对齐	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1|=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道9的采样时间
	ADC1->SMPR2&=~(7<<3);  //通道9采样时间清空	  
 	ADC1->SMPR2|=7<<3;     //通道9 239.5周期,提高采样时间可以提高精确度	 ，还要加上转换时间(转换+对齐、锁存)12.5周期总共252周期，ADC时钟12M，转换时间为252/12M=21us左右

	ADC1->CR2|=1<<0;	   //开启AD转换器	 
	ADC1->CR2|=1<<3;       //使能复位校准  
	while(ADC1->CR2&1<<3); //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  

}				  
//获得ADC值
//ch:通道值0~13
//返回值：转换结果
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=ch;		  			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}
//获得ADC平均值
//ch:通道值0~13
//times:次数
//返回值：通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 	 

void adc_irq_func(void)
{
    static u16 adcx = 0;
    static u16 adcx1 = 0;
    static float temp = 0;
    static u16 adc_t = 0;
    static u16 led_t = 0;

    adc_t++;

    if (adc_t >= TIMER_MS(&g_tim3, 1000))
    {
        adc_t = 0;
        adcx = Get_Adc_Average(ADC_CH9, 3); // ADC原始值
        //printf("ADC Value: %hu\r\n", adcx);
        LED_SMG_WriteValue(adcx, 0, 4);
        temp = (float)adcx * (3.3 / 4096);  // ADC电压值
        adcx1 = temp;
        // temp -= adcx1;
        LED_SMG_WriteNumDP(4, (u16)temp);
        temp *= 1000;
        //LED_SMG_WriteValue((u16)temp % 1000, 5, 3); // 显示小数点后3位，但是这个函数会自动消除前导零，所以如果小数点后有0就显示不出来了
        LED_SMG_WriteValue_contain_zero((u16)temp % 1000, 5, 3);
        

        // for (u8 i = 0; i < 8; i++)
        // {
        //     LED_SMG_WriteNum(i, i);
        // }
    }

    led_t++;

    if (led_t >= TIMER_MS(&g_tim3, 500))
    {
        led_t = 0;
        LED6 = !LED6;
    }


}











