#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key_led_one.h"
#include "project_log_config.h"
#include "rtc.h"
#include "dac.h"
#include "timer.h"
#include "remote.h"
#include "smg.h"
#include "pwm.h"
#include "adc.h"

#if !defined(LOG_TAG)
#define LOG_TAG                    "MAIN"
#endif
#undef LOG_LVL
#if defined(MAIN_LOG_LVL)
#define LOG_LVL                    MAIN_LOG_LVL
#endif
#include <elog.h>
// STM32F103核心板例程
// 库函数版本例程
/********** mcudev.taobao.com 出品  ********/
void _test_elog(void) {
	/* test log output for all level */
	log_a("Hello EasyLogger!");
	log_e("Hello EasyLogger!");
	log_w("Hello EasyLogger!");
	log_i("Hello EasyLogger!");
	log_d("Hello EasyLogger!");
	log_v("Hello EasyLogger!");
	//    elog_raw("Hello EasyLogger!");
}
int main(void)
{
	u8 t = 0;
	Stm32_Clock_Init(9); // 系统时钟设置
	LED_Init();
	NVIC_Configuration(); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	  // 串口初始化为9600波特率
	RTC_Init();			  // RTC初始化
	elog_init();
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_start();
	_test_elog(); // 测试elog输出
	Remote_Init();
	TIM4_Init(1, 7199);		//f = 72M/((7199+1)*10) = 1khz = 1ms
	LED_SMG_Init();
	
	//TIM2_Cap_Init(0XFFFF, 72 - 1);	 //以1Mhz的频率计数，溢出周期约为 65.5ms。
	Adc_Init();					 // ADC初始化

	while (1)
	{
		delay_ms(30);
	}
}

