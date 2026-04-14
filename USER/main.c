#if !defined(LOG_TAG)
#define LOG_TAG                    "MAIN"
#endif
#undef LOG_LVL
#if defined(MAIN_LOG_LVL)
#define LOG_LVL                    MAIN_LOG_LVL
#endif
#include <elog.h>

#include "sys.h"
#include "key_led_one.h"
#include "project_log_config.h"
#include "dac.h"
#include "test_vector.h"
#include "test_quickSort.h"
#include "test_compare_func.h"
#include "tools.h"
#include "beep.h"
#include "OOP/oop_test.h"
#include "test_multi_timer.h"
#include "delay.h"

#define u64 uint64_t
static void esp32_uart_service(void)
{
    u16 sta;
    u16 len;

    sta = USART2_RX_STA;
    if ((sta & 0x8000) == 0)
    {
        return;
    }

    len = sta & 0x3FFF;
    USART2_RX_STA = 0;

    uart2_send_buf((const u8 *)"STM32_ACK:", 10);
    if (len > 0)
    {
        uart2_send_buf(USART2_RX_BUF, len);
    }
    uart2_send_buf((const u8 *)"\r\n", 2);

    printf("[USART2 RX] len=%u\r\n", len);
}

int main(void)
{
    u64 now_ms;
    u64 last_led_ms = 0;
    u64 last_log_ms = 0;
    u8 led_on = 0;

    NVIC_Configuration();
    debug_init();
    _test_elog();
    //test_xvector();
    //test_arr_quicksort();
    //test_xcompare();
    oop_test_all();

    while (1)
    {
        esp32_uart_service();

        now_ms = delay_get_ms();
        if ((now_ms - last_led_ms) >= 500)
        {
            last_led_ms = now_ms;
            led_on = !led_on;
            LED0 = led_on ? 0 : 1;
        }

        if ((now_ms - last_log_ms) >= 1000)
        {
            last_log_ms = now_ms;
            log_d("current time: %llu\r\n", now_ms);
        }
    }
}
