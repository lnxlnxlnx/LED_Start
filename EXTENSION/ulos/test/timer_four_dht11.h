#ifndef B8A72D46_C7ED_48C7_94E8_E7B73FEF3D87
#define B8A72D46_C7ED_48C7_94E8_E7B73FEF3D87
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dht11.h"
#include "timer.h"
#include "smg.h"
#include "ul_timer.h"

// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,全灭
u8 smg_num[] = {0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0x01, 0x00};

u8 smg_wei = 2;  // 数码管位选
u8 num = 0;      // 数值
u16 led_t = 0;   // led统计时间
u16 dht11_t = 0; // dht11采样时间
u8 temperature = 10;  // 温度值
u8 humidity = 20;     // 湿度值
static void time_dht11_callback(void *parameter)
{
    ul_enter_critical();
    DHT11_Read_Data(&temperature, &humidity); // 读取温湿度值
    ul_exit_critical();
}

static void time_smg_callback(void *parameter)
{
    switch (smg_wei)
    {
    case 2:
        num = smg_num[temperature / 10];
        break; // 温度值

    case 3:
        num = smg_num[temperature % 10];
        break;

    case 6:
        num = smg_num[humidity / 10];
        break; // 湿度值

    case 7:
        num = smg_num[humidity % 10];
        break;
    }

    LED_Write_Data(num, smg_wei);
    LED_Refresh();
    smg_wei++;

    if (smg_wei == 4)
        smg_wei = 6;

    if (smg_wei == 8)
        smg_wei = 2;

    led_t++;

    if (led_t == 250) // LED0每500ms闪烁
    {
        led_t = 0;
        LED0 = !LED0;
    }
}
void dht11_example(void)
{
    LED_SMG_Init();
    while (DHT11_Init()) // DHT11初始化
    {
        printf("DHT11 Error\r\n");
        ul_thread_delay(200);
        LED3 = !LED3; // LED3闪烁表示DHT11初始化失败
    }

    LED3 = 1;
    printf("DHT11 OK\r\n");
    /* 创建定时器1  周期定时器 */
    ul_timer_t *timer_dht11_p = ul_timer_create("timer1", time_dht11_callback,
                                                UL_NULL, 500,
                                                ULOS_TIMER_TYPE_PERIODIC);
    ul_timer_t *timer_smg_p = ul_timer_create("timer2", time_smg_callback,
                                              UL_NULL, 2,
                                              ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器1 */
    if (timer_dht11_p != UL_NULL)
        ul_timer_start(timer_dht11_p);

    /* 启动定时器2 */
    if (timer_smg_p != UL_NULL)
        ul_timer_start(timer_smg_p);
    printf("DHT11 Timer Started\r\n");
    // while (1)
    // {
    //     delay_ms(10);
    // }
}

#endif /* B8A72D46_C7ED_48C7_94E8_E7B73FEF3D87 */
