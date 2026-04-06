#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PCout(0)	// PC0
#define LED1 PCout(1)	// PC1
#define LED2 PCout(2)	// PC2
#define LED3 PCout(3)	// PC3
#define LED4 PCout(4)	// PC4
#define LED5 PCout(5)	// PC5
#define LED6 PCout(6)	// PC6
#define LED7 PCout(7)	// PC7

/* NOTE: LED(n, val) 支持 n 为变量（0~7），内部仍为直接寄存器写入。
n 是变量或表达式（例如 i、7-i）不能直接用 __LED_WRITE_CONST。
因为会展开成 LEDi / LED7-i 这种非法标识符，不会变成 LED3。 */
#define __LED_WRITE_CONST(n, val) do { LED##n = (val); } while (0)
#define LED(n, val) \
    do { \
        uint8_t __led_idx = (uint8_t)(n); \
        uint8_t __led_val = (uint8_t)(val); \
        switch (__led_idx) { \
        case 0: __LED_WRITE_CONST(0, __led_val); break; \
        case 1: __LED_WRITE_CONST(1, __led_val); break; \
        case 2: __LED_WRITE_CONST(2, __led_val); break; \
        case 3: __LED_WRITE_CONST(3, __led_val); break; \
        case 4: __LED_WRITE_CONST(4, __led_val); break; \
        case 5: __LED_WRITE_CONST(5, __led_val); break; \
        case 6: __LED_WRITE_CONST(6, __led_val); break; \
        case 7: __LED_WRITE_CONST(7, __led_val); break; \
        default: break; \
        } \
    } while (0)

void LED_Init(void);//初始化

/* LED操作函数封装 */
void led0_operate(uint8_t val);//DEPRECATED  
void led1_operate(uint8_t val);
void led2_operate(uint8_t val);
void led3_operate(uint8_t val);
void led4_operate(uint8_t val);
void led5_operate(uint8_t val);
void led6_operate(uint8_t val);
void led7_operate(uint8_t val);

typedef void (*LED_Operate)(uint8_t val);

extern LED_Operate led_funcs[];

void led_loop_control(void);//测试用循环控制函数
		 				    
#endif

