#include "timer.h"
#include "led.h"
#include "beep.h"
#include "ul_thread.h"
#include "smg.h"
#include "elog.h"
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32开发板
//通用定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/3/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////     


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
//APB1	36MHz	TIM3、TIM6、TIM7
//APB2	72MHz	TIM1、TIM8

void TIM3_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 1;	//TIM3时钟使能    
	TIM3->ARR = arr;  	//设定计数器自动重装值 
	TIM3->PSC = psc;  	//预分频器设置
	TIM3->DIER |= 1 << 0;   //允许更新中断				
	TIM3->CR1 |= 0x01;    //使能定时器3
	MY_NVIC_Init(0, 3, TIM3_IRQn, 2);//抢占0，子优先级3，组2		

}

void TIM4_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 2;	//TIM4时钟使能    
	TIM4->ARR = arr;  	//设定计数器自动重装值 
	TIM4->PSC = psc;  	//预分频器设置
	TIM4->DIER |= 1 << 0;   //允许更新中断				
	TIM4->CR1 |= 0x01;    //使能定时器4
	MY_NVIC_Init(0, 3, TIM4_IRQn, 2);//抢占0，子优先级3，组2		

}

void TIM6_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 4;	//TIM6时钟使能    
	TIM6->ARR = arr;  	//设定计数器自动重装值 
	TIM6->PSC = psc;  	//预分频器设置
	TIM6->DIER |= 1 << 0;   //允许更新中断		
	TIM6->CR1 |= 0x01;
	MY_NVIC_Init(0, 3, TIM6_IRQn, 2);//抢占0，子优先级3，组2	
	//这里应该是TIM6_DAC_IRQn
}

void TIM7_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 5;	//TIM7时钟使能    
	TIM7->ARR = arr;  	//设定计数器自动重装值 
	TIM7->PSC = psc;  	//预分频器设置
	TIM7->DIER |= 1 << 0;   //允许更新中断		
	TIM7->CR1 |= 0x01;
	MY_NVIC_Init(0, 3, TIM7_IRQn, 2);//抢占0，子优先级3，组2		
}

//
void TIM6_IRQHandler(void) //TIM6中断
{
	if (TIM6->SR & 0X0001)//溢出中断
	{
		ul_tick_increase();
	}
	TIM6->SR &= ~(1 << 0);//清除中断标志位 
}

void TIM7_IRQHandler(void) //TIM7中断
{
	if (TIM7->SR & 0X0001)//溢出中断
	{
		ul_tick_increase();
	}
	TIM7->SR &= ~(1 << 0);//清除中断标志位 
}

//定时器2通道1输入捕获配置
void TIM2_Cap_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 0;   	//TIM2 时钟使能 
	RCC->APB2ENR |= 1 << 2;    	//使能PORTA时钟  

	GPIOA->CRL &= 0XFFFFFFF0;	//PA0 清除之前设置  
	GPIOA->CRL |= 0X00000008;	//PA0 输入   
	GPIOA->ODR |= 0 << 0;		//PA0 下拉

	TIM2->ARR = arr;  		//设定计数器自动重装值   
	TIM2->PSC = psc;  		//预分频器 

	TIM2->CCMR1 |= 1 << 0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2->CCMR1 |= 1 << 4; 		//IC1F=0001 配置输入滤波器 以Fck_int采样，2个事件后有效
	TIM2->CCMR1 |= 0 << 10; 	//IC2PS=00 	配置输入分频,不分频 

	TIM2->CCER |= 0 << 1; 		//CC1P=0	上升沿捕获
	TIM2->CCER |= 1 << 0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM2->DIER |= 1 << 1;   	//允许捕获中断				
	TIM2->DIER |= 1 << 0;   	//允许更新中断	
	TIM2->CR1 |= 0x01;    	//使能定时器2
	MY_NVIC_Init(2, 0, TIM2_IRQn, 2);//抢占2，子优先级0，组2

}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到高电平;1,已经捕获到高电平了.
//[5:0]:捕获高电平后溢出的次数
u8  TIM2CH1_CAPTURE_STA = 0;	//输入捕获状态		    				
u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值
//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{
	u16 tsr;
	tsr = TIM2->SR;
	if ((TIM2CH1_CAPTURE_STA & 0X80) == 0)//还未成功捕获	
	{
		if (tsr & 0X01)//溢出
		{
			if (TIM2CH1_CAPTURE_STA & 0X40)//已经捕获到高电平了
			{
				if ((TIM2CH1_CAPTURE_STA & 0X3F) == 0X3F)//高电平太长了
				{
					TIM2CH1_CAPTURE_STA |= 0X80;//标记成功捕获了一次
					TIM2CH1_CAPTURE_VAL = 0XFFFF;
				}
				else TIM2CH1_CAPTURE_STA++;
			}
		}
		if (tsr & 0x02)//捕获1发生捕获事件
		{
			if (TIM2CH1_CAPTURE_STA & 0X40)		//捕获到一个下降沿 		
			{
				TIM2CH1_CAPTURE_STA |= 0X80;		//标记成功捕获到一次高电平脉宽
				TIM2CH1_CAPTURE_VAL = TIM2->CCR1;	//获取当前的捕获值.
				TIM2->CCER &= ~(1 << 1);			//CC1P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				TIM2CH1_CAPTURE_VAL = 0;
				TIM2CH1_CAPTURE_STA = 0X40;		//标记捕获到了上升沿
				TIM2->CNT = 0;					//计数器清空
				TIM2->CCER |= 1 << 1; 				//CC1P=1 设置为下降沿捕获 
			}
		}
	}
	TIM2->SR = 0;//清除中断标志位 	    
}

//定时器3中断服务程序	 
extern void remote_irq_func(void);
extern void led_irq_func(void);
/**
 * @description: 每10毫秒执行一次
 * @return {*}
 */
uint16_t TIM2_SIG_HIGH_TIME = 0;
void TIM2_Input_Capture_Update()
{
	static uint32_t temp = 0;
	static uint8_t t = 0;
	//static uint8_t cnt_to_1ms = 0;
	//cnt_to_1ms++;
	t++;
	if (t == 19)
	{
		t = 0;
		LED0 = !LED0;
	}
	if (TIM2CH1_CAPTURE_STA & 0X80) //成功捕获到了一次高电平
	{
		temp = TIM2CH1_CAPTURE_STA & 0X3F;
		temp *= 65536;					//溢出时间总和
		temp += TIM2CH1_CAPTURE_VAL;		//得到总的高电平时间
		printf("HIGH:%d us\r\n", temp);	//打印总的高点平时间
		TIM2_SIG_HIGH_TIME = temp/1000;
		TIM2CH1_CAPTURE_STA = 0;			//开启下一次捕获
	}
}
#define USE_LED 1
#define USE_REMOTE 1
#define USE_IC_UPDATE 1
void TIM3_IRQHandler(void)
{
#if USE_REMOTE
	remote_irq_func();
#endif

#if USE_LED
	led_irq_func();
#endif
#if USE_IC_UPDATE
	TIM2_Input_Capture_Update();
#endif
	TIM3->SR = 0;//清除中断标志位   
}
#ifdef USE_REMOTE
#undef USE_REMOTE
#endif

extern u16 TIM3_ONE_SECOND_COUNT;
/**
 * @description: 用于数码管显示的更新
 * @return {*}
 */
void TIM4_IRQHandler(void) //TIM4中断
{
	// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .，全灭
	static u8 smg_num[] = { 0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0x01, 0x00 };
	static u8 key = 0; //按键值
	static u8 num = 0x00; //数值
	static u8 num1 = 0x00; //数值
	static u8 num2 = 0x00; //数值
	static u8 num3 = 0x00; //数值
	static u8 smg_wei = 6; //数码管位选
	static u8 smg_duan = 0; //数码管段选
	static u8 smg_flag = 0; //数码管显示标志 0:正常显示 1:不显示（消除鬼影）
	static u8 t = 0;

	static u8 press_change_period = 0;

	// trick
	// num3 = TIM2_SIG_HIGH_TIME / 1000; //高电平时间的秒位
	// num2 = (TIM2_SIG_HIGH_TIME % 1000) / 100; //高电平时间的毫秒位
	// num1 = (TIM2_SIG_HIGH_TIME % 100) / 10; //高电平时间的百分位
	// num = (TIM2_SIG_HIGH_TIME % 10); //高电平时间的个位

	if (TIM4->SR & 0X0001) //溢出中断
	{
		//key = Remote_Scan();//获取红外遥控键值
		//key = TIM2_SIG_HIGH_TIME; //获取输入捕获的高电平时间

		if (key)
		{
			switch (key)
			{
				case 104:
					num1 = 0x00;
					num = smg_num[1];
					BEEP = 1;
					break; //按键'1'

				case 152:
					num1 = 0x00;
					num = smg_num[2];
					BEEP = 1;
					break;     //按键'2'

				case 176:
					num1 = 0x00;
					num = smg_num[3];
					BEEP = 0;
					break;     //按键'3'

				case 48:
					num1 = 0x00;
					num = smg_num[4];
					BEEP = 0;
					break;      //按键'4'

				case 24:
					num1 = 0x00;
					num = smg_num[5];
					BEEP = 0;
					break;      //按键'5'

				case 122:
					num1 = 0x00;
					num = smg_num[6];
					BEEP = 0;
					break;     //按键'6'

				case 16:
					num1 = 0x00;
					num = smg_num[7];
					BEEP = 0;
					break;      //按键'7'

				case 56:
					num1 = 0x00;
					num = smg_num[8];
					BEEP = 0;
					break;      //按键'8'

				case 90:
					num1 = 0x00;
					num = smg_num[9];
					BEEP = 0;
					break;      //按键'9'

				case 66:
					num1 = 0x00;
					num = smg_num[0];
					BEEP = 0;
					break;      //按键'0'

				case 82:
					num1 = 0x00;
					num = smg_num[17];
					BEEP = 0;
					break;     //按键'DELETE'

				case 162:
					num1 = smg_num[1];
					num = smg_num[0];
					BEEP = 0;
					break;//按键'POWER'

				case 98:
					num1 = smg_num[1];
					num = smg_num[1];
					BEEP = 0;
					break;//按键'UP'

				case 226:
					num1 = smg_num[1];
					num = smg_num[2];
					BEEP = 0;
					break;//按键'ALIENTEK'

				case 34:
					num1 = smg_num[1];
					num = smg_num[3];
					BEEP = 0;
					break;//按键'LEFT'

				case 2:
					num1 = smg_num[1];
					num = smg_num[4];
					BEEP = 0;
					break;//按键'PLAY'

				case 194:
					num1 = smg_num[1];
					num = smg_num[5];
					BEEP = 0;
					break;//按键'RIGHT'

				case 224:
					//num1 = smg_num[1];
					//num = smg_num[6];
					//TIM3_ONE_SECOND_COUNT -= 200;
					press_change_period = 2;
					BEEP = 1;
					break;//按键'VOL-'

				case 168:
					num1 = smg_num[1];
					num = smg_num[7];
					BEEP = 0;
					break;//按键'DOWN'

				case 144:
					//num1 = smg_num[1];
					//num = smg_num[8];
					//TIM3_ONE_SECOND_COUNT += 200;
					press_change_period = 1;
					//if (TIM3_ONE_SECOND_COUNT > 2000) TIM3_ONE_SECOND_COUNT = 2000; //限制最大值
					BEEP = 1;
					break;//按键'VOL+'
			}
		}
		else
		{
			BEEP = 1;
		}
		if (TIM3_ONE_SECOND_COUNT < 200) TIM3_ONE_SECOND_COUNT = 200;
		else if (TIM3_ONE_SECOND_COUNT > 5000) TIM3_ONE_SECOND_COUNT = 5000;
		//u16 temp = TIM3_ONE_SECOND_COUNT;
		u16 temp = TIM2_SIG_HIGH_TIME;
		num = smg_num[temp % 10];
		temp = temp / 10;
		num1 = smg_num[temp % 10];
		temp = temp / 10;
		num2 = smg_num[temp % 10];
		temp = temp / 10;
		num3 = smg_num[temp % 10];


		if (smg_wei == 6) //数码管位
		{
			smg_duan = num1;
		}
		else if (smg_wei == 7) //数码管位
		{
			smg_duan = num;
		}
		else if (smg_wei == 5) //数码管位
		{
			smg_duan = num2;
		}
		else if (smg_wei == 4) //数码管位
		{
			smg_duan = num3;
		}

		if (smg_flag) LED_Write_Data(0x00, smg_wei); //消除鬼影(段码不显示)
		else 	  LED_Write_Data(smg_duan, smg_wei); //正常显示

		LED_Refresh();//数码管数据更新
		smg_flag = !smg_flag;

		if (smg_flag == 0) //正常显示才更新位码
		{
			smg_wei++;

			if (smg_wei == 8) smg_wei = 4;
		}

		t++;

		if (t == 250) //LED1每500MS闪烁
		{
			t = 0;
			LED7 = !LED7;
			switch (press_change_period)
			{
				case 1:
					//log_i("current num = %#03x", num);
					TIM3_ONE_SECOND_COUNT += 200;
					break;
				case 2:
					//log_i("current num = %#03x", num1);
					TIM3_ONE_SECOND_COUNT -= 200;
					break;
				case 3:
					log_i("current num = %#03x", num2);
					break;
				case 4:
					log_i("current num = %#03x", num3);
					break;
				default:
					break;
			}
			press_change_period = 0;
		}
	}

	TIM4->SR &= ~(1 << 0); //清除中断标志位

}



