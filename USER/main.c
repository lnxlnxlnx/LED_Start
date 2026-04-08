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

int main(void)
{
    NVIC_Configuration();
    debug_init();
    _test_elog();
    //test_xvector();
    //test_arr_quicksort();
    //test_xcompare();
    oop_test_all();

    while (1)
    {
        //key_led_one_loop(0);
        //MultiTimerYield();
        //BEEP = 0;
        LED0 = 0;
        delay_ms(500);
        LED0 = 1;
        delay_ms(500);
        log_d( "current time: %llu\r\n", delay_get_ms());
    }
}
