#ifndef FAABE45E_4986_45FD_BD2C_FFC38B7ED3B5
#define FAABE45E_4986_45FD_BD2C_FFC38B7ED3B5
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "ds18b20.h"
#include "timer.h"
#include "smg.h"
#include "ul_timer.h"

// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,全灭
u8 smg_num[] = {0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0x01, 0x00};

short temperature; // 温度值
u8 smg_wei = 4;    // 数码管位选
u8 num = 0;        // 数码管数值
u16 led_t = 0;     // led时间
u16 temp_t = 0;    // 采样时间
u8 flag = 0;       // 温度正负标志位
static void time_dh18b20_callback(void *parameter)
{
    temperature = DS18B20_Get_Temp(); // 获取温度值

    if (temperature < 0) // 若温度为负数
    {
        temperature = -temperature;
        flag = 1;
    }
    else
        flag = 0;
}

static void time_smg_callback(void *parameter)
{
    switch (smg_wei)
    {
    case 4:
        if (flag)
            num = 0x02; // 显示"-"以表示负温度
        else
            num = 0x00;

        break;

    case 5:
        num = smg_num[temperature / 10 / 10];
        break; // 温度值

    case 6:
        num = smg_num[temperature / 10 % 10] | 0x01;
        break;

    case 7:
        num = smg_num[temperature % 10];
        break;
    }

    LED_Write_Data(num, smg_wei);
    LED_Refresh();
    smg_wei++;

    if (smg_wei == 8)
        smg_wei = 4;

    led_t++;

    if (led_t == 250) // LED0 500ms闪烁
    {
        led_t = 0;
        LED0 = !LED0;
    }
}

void ds18b20_example(void)
{
    LED_SMG_Init();
    while (DS18B20_Init()) // DS18B20初始化
    {
        printf("DS18B20 Error\r\n");
        delay_ms(200);
        LED3 = !LED3; // LED3闪烁表示DS18B20初始化失败
    }

    LED3 = 1;
    printf("DS18B20 OK\r\n");
    /* 创建定时器1  周期定时器 */
    ul_timer_t *timer_dh18b20_p = ul_timer_create("timer1", time_dh18b20_callback,
                                                  UL_NULL, 500,
                                                  ULOS_TIMER_TYPE_PERIODIC);
    ul_timer_t *timer_smg_p = ul_timer_create("timer2", time_smg_callback,
                                              UL_NULL, 2,
                                              ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器1 */
    if (timer_dh18b20_p != UL_NULL)
        ul_timer_start(timer_dh18b20_p);

    /* 启动定时器2 */
    if (timer_smg_p != UL_NULL)
        ul_timer_start(timer_smg_p);

    // while (1)
    // {
    //     delay_ms(10);
    // }
}

#endif /* FAABE45E_4986_45FD_BD2C_FFC38B7ED3B5 */
