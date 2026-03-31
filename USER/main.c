#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key_led_one.h"
//#define LOG_TAG "MAIN"
/**
 * Log default configuration for EasyLogger.
 * NOTE: Must defined before including the <elog.h>
 */
#define MAIN_LOG_LVL ELOG_LVL_WARN
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
	elog_init();

	elog_start();
	_test_elog(); // 测试elog输出

	while (1)
	{
		key_led_one_loop();
		//		for (uint8_t i = 0; i < 8; i++)
		//		{
		//			led_funcs[i](0);
		//			delay_ms(200);
		//			led_funcs[i](1);
		//		}
	}
}
