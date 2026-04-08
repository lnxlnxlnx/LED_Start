#ifndef B50D49D1_5097_439F_AFB6_5970F7F57A01
#define B50D49D1_5097_439F_AFB6_5970F7F57A01
#include "MultiTimer.h"
#include "usart.h"
#include "delay.h"

MultiTimer timer1;
MultiTimer timer2;
MultiTimer timer3;

static volatile u16 cnt1 = 0;
void exampleTimer1Callback(MultiTimer* timer, void *userData)
{
    ++cnt1;
    printf("exampleTimer1Callback-> %s.\r\n, current time: %lu  cnt: %u\r\n", (char*)userData, delay_get_ms(), cnt1);
    MultiTimerStart(timer, 1000, exampleTimer1Callback, userData);
}

void exampleTimer2Callback(MultiTimer* timer, void *userData)
{
    printf("exampleTimer2Callback-> %s.\r\n", (char*)userData);
}

void exampleTimer3Callback(MultiTimer* timer, void *userData)
{
    printf("exampleTimer3Callback-> %s.\r\n", (char*)userData);
    MultiTimerStart(timer, 4567, exampleTimer3Callback, userData);
}

void test_multi_timer(void)
{
    MultiTimerStart(&timer1, 1000, exampleTimer1Callback, "1000ms CYCLE timer");
    //MultiTimerStart(&timer2, 5000, exampleTimer2Callback, "5000ms ONCE timer");
    //MultiTimerStart(&timer3, 3456, exampleTimer3Callback, "3456ms delay start, 4567ms CYCLE timer");
}

#endif /* B50D49D1_5097_439F_AFB6_5970F7F57A01 */
