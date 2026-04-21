#ifndef C0739F6A_3FF0_481F_9DE9_B26FBAB8C1CB
#define C0739F6A_3FF0_481F_9DE9_B26FBAB8C1CB
#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"

// 全局变量声明
extern uint16_t LED_DELAY_TIME;

/***************************************
1.列出所有的状态
***************************************/
typedef enum
{
    FALLING,
    RISING,
    CIRCLE,
    STOP
} STATE_TYPE;

typedef enum{
    NORMAL,
    BLINK,
    CIRCLE2
} LED_BEEP_MODE;

/***************************************
2.列出所有的事件
***************************************/
typedef enum
{
    KNONE,
    K0,
    K1,
    K2,
    kUP
} EVENT_TYPE;

/***************************************
3.定义和状态机相关结构
***************************************/
typedef struct
{
    STATE_TYPE state; // 当前状态
    EVENT_TYPE event; // 当前事件
} STATE_MACHINE;

typedef struct
{
    LED_BEEP_MODE mode;
    EVENT_TYPE event;    
} LED_BEEP_MACHINE;

// 全局结构体声明
extern STATE_MACHINE state_machine;
extern LED_BEEP_MACHINE led_beep_machine;
// 函数声明（仅声明，实现移到key_led_one.c）
void key_led_one_loop(void);
void led_beep_loop(void);

#endif /* C0739F6A_3FF0_481F_9DE9_B26FBAB8C1CB */

