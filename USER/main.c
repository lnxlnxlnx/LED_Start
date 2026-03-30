#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key_led_one.h"

// STM32F103核心板例程
// 库函数版本例程
/********** mcudev.taobao.com 出品  ********/

int main(void)
{
	u8 t = 0;
	delay_init(); // 延时函数初始化
	LED_Init();
	NVIC_Configuration(); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	  // 串口初始化为9600

	while (1)
	{
		key_led_one_loop();
		for (uint8_t i = 0; i < 8; i++)
		{
			led_funcs[i](0);
			delay_ms(200);
			led_funcs[i](1);
		}
	}
}
