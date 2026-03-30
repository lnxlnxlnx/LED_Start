#ifndef B9D6FB98_F713_498F_936D_841B61DF300D
#define B9D6FB98_F713_498F_936D_841B61DF300D
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usmart.h"
#include "rtc.h"
#include "smg.h"
#include "timer.h"
#include "ul_thread.h"
#include "ul_timer.h"
#include <elog.h>

void test_elog(void) {
    /* test log output for all level */
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
//    elog_raw("Hello EasyLogger!");
}

static void time_rtc_callback(void *parameter)
{
    test_elog(); // 测试elog输出
}

static void time_smg_callback(void *parameter)
{
    // 共阴数字数组
    // 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,全灭
    static u8 smg_num[] = {0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0x01, 0x00};

    static u8 smg_wei = 0; // 数码管位选
    static u8 num = 0;     // 数码管数值
    static u8 time = 0;    // 时间值

    switch (smg_wei)
    {
    case 0:
        num = smg_num[calendar.hour / 10];
        break; // 时

    case 1:
        num = smg_num[calendar.hour % 10];
        break;

    case 2:
    case 5:
        num = 0x02;
        break;

    case 3:
        num = smg_num[calendar.min / 10];
        break; // 分

    case 4:
        num = smg_num[calendar.min % 10];
        break;

    case 6:
        num = smg_num[calendar.sec / 10];
        break; // 秒

    case 7:
        num = smg_num[calendar.sec % 10];
        break;
    }

    if (time != calendar.sec) // LED0每秒闪烁
    {
        time = calendar.sec;
        LED0 = !LED0;
    }

    LED_Write_Data(num, smg_wei); // 写数据
    LED_Refresh();                // 更新显示
    smg_wei++;                    // 数码管位选变化

    if (smg_wei == 8)
        smg_wei = 0;
}

void rtc_example(void)
{
    while (RTC_Init()) // RTC初始化，一定要初始化成功
    {
        printf("RTC ERROR!\r\n");
        delay_ms(800);
        printf("RTC Trying...\r\n");
    }
        /* set EasyLogger log format */
    /*
    ASSERT  ：全部信息（最全）
    ERROR   ：时间 + 级别 + 标签
    WARN    ：时间 + 级别 + 标签
    INFO    ：时间 + 级别 + 标签
    DEBUG   ：时间+级别+标签+文件名+行号（无函数/线程/进程）
    VERBOSE ：同 DEBUG
    */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    /* start EasyLogger */
    ul_timer_t *timer_rtc_p = ul_timer_create("timer1", time_rtc_callback,
                                              UL_NULL, 3000,
                                              ULOS_TIMER_TYPE_PERIODIC);
    ul_timer_t *timer_smg_p = ul_timer_create("timer2", time_smg_callback,
                                              UL_NULL, 2,
                                              ULOS_TIMER_TYPE_PERIODIC);

    /* 启动定时器1 */
    if (timer_rtc_p != UL_NULL)
        ul_timer_start(timer_rtc_p);

    /* 启动定时器2 */
    if (timer_smg_p != UL_NULL)
        ul_timer_start(timer_smg_p);
}
#endif /* B9D6FB98_F713_498F_936D_841B61DF300D */
