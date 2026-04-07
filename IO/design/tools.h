#ifndef BCAF113D_5FEF_4C39_8344_98DFC697FB50
#define BCAF113D_5FEF_4C39_8344_98DFC697FB50
#include "usart.h"
#include "elog.h"
#include "device_init_factory.h"
void debug_init(void){
    int ret;
    uart_init(115200);

    elog_init();
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_start();

    device_set_hook(_device_init_hook); // 设置设备初始化钩子函数，打印设备初始化日志用的
    ret = device_create(device_all_mask() & (~DEV_KEY_EXTI)); // 创建设备，DAC设备不创建，测试一下设备初始化失败的日志
    if (ret != DEVICE_OK)
    {
        log_e("device_create failed: ret=%d", ret);
        while (1);
    }
}

void _test_elog(void)
{
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
}

#endif /* BCAF113D_5FEF_4C39_8344_98DFC697FB50 */
