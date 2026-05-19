#ifndef DA26C183_5903_480D_BB72_26351314818A
#define DA26C183_5903_480D_BB72_26351314818A
#include "timer.h"
#include "led.h"
#include "smg.h"
#include "ul_thread.h"
#include "ul_timer.h"
#include "beep.h"
#include "adc.h"

static ul_timer_t *timer_smg_p = UL_NULL;

// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .，全灭
u8 smg_num[] = {0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0x01, 0x00};
u8 key = 0;      // 按键值
u8 num = 0x00;   // 数值
u8 num1 = 0x00;  // 数值
u8 smg_wei = 6;  // 数码管位选
u8 smg_duan = 0; // 数码管段选
u8 smg_flag = 0; // 数码管显示标志 0:正常显示 1:不显示（消除鬼影）
u8 t = 0;

static void time_smg_callback(void *parameter)
{
    LED_Write_Data(smg_num[smg_duan], smg_wei); // 数码管显示
    LED_Refresh();                              // 刷新显示
    smg_wei++;

    if (smg_wei == 8)
        smg_wei = 0;

    t++;

    if (t == 500) // 数码管每秒更新
    {
        t = 0;
        LED0 = !LED0;
        smg_duan++;

        if (smg_duan == 18)
            smg_duan = 0;
    }
}

void smg_example(void)
{
    LED_SMG_Init();
    /* 创建定时器1  周期定时器 */
    timer_smg_p = ul_timer_create("timer1", time_smg_callback,
                                  UL_NULL, 2,
                                  ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器1 */
    if (timer_smg_p != UL_NULL)
        ul_timer_start(timer_smg_p);
}

extern u8 TIM2CH1_CAPTURE_STA;  // 输入捕获状态
extern u16 TIM2CH1_CAPTURE_VAL; // 输入捕获值
void thread_capture_entry(void *p)
{
    u16 t = 0;
    u32 temp = 0;
    while (1)
    {
        ul_thread_delay(10);

        if (TIM2CH1_CAPTURE_STA & 0X80) // 成功捕获到了一次高电平
        {
            temp = TIM2CH1_CAPTURE_STA & 0X3F;
            temp *= 65536;                  // 溢出时间总和
            temp += TIM2CH1_CAPTURE_VAL;    // 得到总的高电平时间
            printf("HIGH:%d us\r\n", temp); // 打印总的高点平时间
            TIM2CH1_CAPTURE_STA = 0;        // 开启下一次捕获
        }

        t++;

        if (t == 20)
        {
            t = 0;
            LED1 = !LED1;
        }
    }
}

static void time_remote_callback(void *parameter)
{
    key = Remote_Scan(); // 获取红外遥控键值

    if (key)
    {
        switch (key)
        {
        case 104:
            num1 = 0x00;
            num = smg_num[1];
            BEEP = 0;
            break; // 按键'1'

        case 152:
            num1 = 0x00;
            num = smg_num[2];
            BEEP = 0;
            break; // 按键'2'

        case 176:
            num1 = 0x00;
            num = smg_num[3];
            BEEP = 0;
            break; // 按键'3'

        case 48:
            num1 = 0x00;
            num = smg_num[4];
            BEEP = 0;
            break; // 按键'4'

        case 24:
            num1 = 0x00;
            num = smg_num[5];
            BEEP = 0;
            break; // 按键'5'

        case 122:
            num1 = 0x00;
            num = smg_num[6];
            BEEP = 0;
            break; // 按键'6'

        case 16:
            num1 = 0x00;
            num = smg_num[7];
            BEEP = 0;
            break; // 按键'7'

        case 56:
            num1 = 0x00;
            num = smg_num[8];
            BEEP = 0;
            break; // 按键'8'

        case 90:
            num1 = 0x00;
            num = smg_num[9];
            BEEP = 0;
            break; // 按键'9'

        case 66:
            num1 = 0x00;
            num = smg_num[0];
            BEEP = 0;
            break; // 按键'0'

        case 82:
            num1 = 0x00;
            num = smg_num[17];
            BEEP = 0;
            break; // 按键'DELETE'

        case 162:
            num1 = smg_num[1];
            num = smg_num[0];
            BEEP = 0;
            break; // 按键'POWER'

        case 98:
            num1 = smg_num[1];
            num = smg_num[1];
            BEEP = 0;
            break; // 按键'UP'

        case 226:
            num1 = smg_num[1];
            num = smg_num[2];
            BEEP = 0;
            break; // 按键'ALIENTEK'

        case 34:
            num1 = smg_num[1];
            num = smg_num[3];
            BEEP = 0;
            break; // 按键'LEFT'

        case 2:
            num1 = smg_num[1];
            num = smg_num[4];
            BEEP = 0;
            break; // 按键'PLAY'

        case 194:
            num1 = smg_num[1];
            num = smg_num[5];
            BEEP = 0;
            break; // 按键'RIGHT'

        case 224:
            num1 = smg_num[1];
            num = smg_num[6];
            BEEP = 0;
            break; // 按键'VOL-'

        case 168:
            num1 = smg_num[1];
            num = smg_num[7];
            BEEP = 0;
            break; // 按键'DOWN'

        case 144:
            num1 = smg_num[1];
            num = smg_num[8];
            BEEP = 0;
            break; // 按键'VOL+'
        }
    }
    else
    {
        BEEP = 1;
    }

    if (smg_wei == 6) // 数码管位
    {
        smg_duan = num1;
    }
    else if (smg_wei == 7) // 数码管位
    {
        smg_duan = num;
    }

    if (smg_flag)
        LED_Write_Data(0x00, smg_wei); // 消除鬼影(段码不显示)
    else
        LED_Write_Data(smg_duan, smg_wei); // 正常显示

    LED_Refresh(); // 数码管数据更新
    smg_flag = !smg_flag;

    if (smg_flag == 0) // 正常显示才更新位码
    {
        smg_wei++;

        if (smg_wei == 8)
            smg_wei = 6;
    }

    t++;

    if (t == 250) // LED3每500MS闪烁
    {
        t = 0;
        LED3 = !LED3;
    }
}

void tim2_capture_example(void)
{
    TIM2_Cap_Init(0XFFFF, 72 - 1); // 以1Mhz的频率计数
    ul_thread_t *tid_capture = ul_thread_create("t0", thread_capture_entry, UL_NULL, 512, 0, 1);
    ul_thread_startup(tid_capture);
}

void remote_example(void)
{
    LED_SMG_Init();
    Remote_Init(); /* 红外接收初始化 */

    /* 创建定时器2  周期定时器 */
    ul_timer_t *timer_remote_p = ul_timer_create("timer2", time_remote_callback,
                                                 UL_NULL, 2,
                                                 ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器2 */
    if (timer_remote_p != UL_NULL)
        ul_timer_start(timer_remote_p);
}

/*** 
 * @description: 用于定时器3周期性采样ADC值并显示在数码管上(200ms)
 * PB1/TIM3_CH4/ADC_IN9连接一个电位器，调节电位器可以改变ADC值从而改变数码管显示的数值(注意跳线帽)
 * @param {void} *parameter
 * @return {*}
 */
static void time_adc_callback(void *parameter)
{
    // ADC变量
    static u16 adcx = 0;
    static u16 adcx1 = 0;
    static float temp = 0;

    static u8 num = 0;     // 数码管值
    static u8 smg_wei = 0; // 数码管位选
    static u8 adc_t = 0;   // adc采样时间值
    static u8 led_t = 0;   // led显示时间值
    adc_t++;

    if (adc_t == 100) // ADC 200ms采样
    {
        adc_t = 0;
        adcx = Get_Adc_Average(ADC_CH9, 3); // ADC原始值
        temp = (float)adcx * (3.3 / 4096);  // ADC电压值
        adcx1 = temp;
        temp -= adcx1;
        temp *= 1000;
    }

    switch (smg_wei)
    {
    // ADC采样原始值
    case 0:
        if (adcx > 1000)
            num = smg_num[adcx / 1000];
        else
            num = 0x00;

        break;

    case 1:
        if (adcx > 100)
            num = smg_num[adcx % 1000 / 100];
        else
            num = 0x00;

        break;

    case 2:
        if (adcx > 10)
            num = smg_num[adcx % 100 / 10];
        else
            num = 0x00;

        break;

    case 3:
        num = smg_num[adcx % 10];
        break;

    case 4:
        num = 0x00;
        break;

    // ADC换算后的电压值
    case 5:
        num = smg_num[adcx1] | 0x01;
        break;

    case 6:
        num = smg_num[(u16)temp / 100];
        break;

    case 7:
        num = smg_num[(u16)temp % 100 / 10];
        break;
    }

    LED_Write_Data(num, smg_wei);
    LED_Refresh();
    smg_wei++;

    if (smg_wei == 8)
        smg_wei = 0;

    led_t++;

    if (led_t == 250) // 500ms闪烁
    {
        led_t = 0;
        LED0 = !LED0;
    }
}

void adc_example(void)
{
    LED_SMG_Init();
    Adc_Init(); /* ADC初始化 */

    /* 创建定时器3  周期定时器 */
    ul_timer_t *timer_adc_p = ul_timer_create("timer3", time_adc_callback,
                                              UL_NULL, 2,
                                              ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器3 */
    if (timer_adc_p != UL_NULL)
        ul_timer_start(timer_adc_p);
}

#endif /* DA26C183_5903_480D_BB72_26351314818A */
