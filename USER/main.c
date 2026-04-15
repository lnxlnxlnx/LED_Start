#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key_led_one.h"
#include "project_log_config.h"
#include "rtc.h"
#include "dac.h"

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
	delay_init(); // 延时函数初始化
	LED_Init();
	NVIC_Configuration(); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	  // 串口初始化为9600
	RTC_Init();			  // RTC初始化
	Dac_Init();		  // DAC初始化
	elog_init();
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_start();
	_test_elog(); // 测试elog输出
	//Generate_SineWave();

	while (1)
	{
		key_led_one_loop();
		// Dac_Set_Vol(2000); // 设置DAC输出电压为2.0V
		// delay_ms(100);
		// Dac_Set_Vol(200); // 设置DAC输出电压为1.0V
		// delay_ms(100);
		//Output_SineWave(); // 输出正弦波
	}
}
