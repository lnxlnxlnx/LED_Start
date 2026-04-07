#ifndef C0465C8B_5934_425B_B377_7D59FDC71CA8
#define C0465C8B_5934_425B_B377_7D59FDC71CA8
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "tpad.h"
#include "ul_thread.h"
void thread_tpad_entry(void *p)
{
    u8 t = 0;
    TPAD_Init(6); // 初始化触摸按键

    while (1)
    {
        if (TPAD_Scan(0)) // 成功捕获到了一次上升沿(此函数执行时间至少15ms)
        {
            LED5 = !LED5; // LED5取反
        }

        t++;

        if (t == 15)
        {
            t = 0;
            LED0 = !LED0; // LED0取反,提示程序正在运行
        }

        //delay_ms(10);
        ul_thread_delay(10);
    }
}
void tpad_example(void)
{
    ul_thread_t *tid_tpad = ul_thread_create("task_tpad", thread_tpad_entry, UL_NULL, 512, 0, 1);
    ul_thread_startup(tid_tpad);
}

#endif /* C0465C8B_5934_425B_B377_7D59FDC71CA8 */
