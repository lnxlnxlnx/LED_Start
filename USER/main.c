#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key_led_one.h"
#include "design/device_factory.h"
#include "project_log_config.h"
#include "dac.h"
#include "test_vector.h"
#include "test_quickSort.h"
#if !defined(LOG_TAG)
    #define LOG_TAG                    "MAIN"
#endif
#undef LOG_LVL
#if defined(MAIN_LOG_LVL)
    #define LOG_LVL                    MAIN_LOG_LVL
#endif
#include <elog.h>

void _test_elog(void)
{
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
}

int main(void)
{
    int ret;

    NVIC_Configuration();
    uart_init(115200);

    elog_init();
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_start();

    device_set_hook(_device_init_hook);
    ret = device_create(device_all_mask());
    if (ret != DEVICE_OK)
    {
        log_e("device_create failed: ret=%d", ret);
        while (1);
    }

    Dac_Init();

    _test_elog();
    test_xvector();
    test_arr_quicksort();

    while (1)
    {
        key_led_one_loop();
    }
}
