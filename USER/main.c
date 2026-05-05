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
	//delay_init(72); // 延时函数初始化
	LED_Init();
	NVIC_Configuration(); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	  // 串口初始化为9600
	RTC_Init();			  // RTC初始化
	Dac_Init();		  // DAC初始化
	elog_init();
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_start();
	_test_elog(); // 测试elog输出
	// 72M 系统时钟
	// TIM3_Init(自动重装载值, 预分频)
	// 1ms 中断配置：
	//TIM3_Init(9, 7199);		//f = 72M/((7199+1)*10) = 1khz = 1ms
	//TIM3_Init(9999, 7199);		//f = 72M/((7199+1)*10000) = 1hz = 1s
	//Remote_Init();
	//LED_SMG_Init();
	//TIM4_Init(19, 7199);		//f = 72M/((7199+1)*2000) = 5hz = 2ms

	TIM3_PWM_Init(899, 0); //不分频。PWM频率=72000/(899+1)=80Khz
	u16 led6pwmval = 0;
	u8 dir = 1;

	while (1)
	{
		delay_ms(10);

		if (dir)led6pwmval++;
		else led6pwmval--;

		if (led6pwmval > 300)dir = 0;

		if (led6pwmval == 0)dir = 1;

		LED6_PWM_VAL = led6pwmval;
	}
}

