#include "device_init_factory.h"
#include "delay.h"
#include "led.h"
#include "exti.h"
#include "key.h"
#include "beep.h"
#include "rtc.h"
#include "project_log_config.h"
#if !defined(LOG_TAG)
    #define LOG_TAG                    "device_factory"
#endif
#undef LOG_LVL
#if defined(DEVICE_FACTORY_LOG_LVL)
    #define LOG_LVL                    DEVICE_FACTORY_LOG_LVL
#endif
#include <elog.h>

typedef struct
{
    /* bit 必须是 2^n 的单比特值 */
    DeviceType bit;
    /* 用于日志/调试输出 */
    const char *name;
    /* 具体设备的初始化函数 */
    device_init_fn_t init;
} DeviceDesc;

static int _device_led_init(void)
{
    LED_Init();
    return DEVICE_OK;
}

static int _device_key_exti_init(void)
{
    KEY_Init();
    EXTIX_Init();
    return DEVICE_OK;
}

static int _device_rtc_init(void)
{
    u8 rtc_ret = RTC_Init();
    /* Project convention: RTC_Init() returns 0 on success. */
    if (rtc_ret == 0u)
    {
        return DEVICE_OK;
    }
    return DEVICE_ERR_INIT_FAILED;
}

static int _device_delay_init(void)
{
    delay_init();
    return DEVICE_OK;
}

static int _device_beep_init(void)
{
    BEEP_Init();
    return DEVICE_OK;
}

/// @brief 设备工厂的静态配置表，描述了工厂支持的全部设备及其初始化函数。
/// NOTE: 初始化就是三步走:init函数、宏定义分配bit、然后填下面这个表
static const DeviceDesc g_device_descs[] =
{
    /* 注意: 初始化顺序由 bit 位决定，不由这里的数组顺序决定。 */
    { DEV_LED,      "LED",      _device_led_init },
    { DEV_KEY_EXTI, "KEY_EXTI", _device_key_exti_init },
    { DEV_RTC,      "RTC",      _device_rtc_init },
    { DEV_DELAY,    "DELAY",    _device_delay_init },
    { DEV_BEEP,     "BEEP",     _device_beep_init }
};

#define DEVICE_DESC_COUNT ((u32)(sizeof(g_device_descs) / sizeof(g_device_descs[0])))

static const DeviceDesc *g_desc_by_bitpos[32];
/* 工厂支持的全部设备 bit 集合，由 g_device_descs 动态构建。 */
static u32 g_known_mask = DEV_NONE;
/* 已经成功初始化过的设备 bit 集合，避免重复初始化。 */
static u32 g_initialized_mask = DEV_NONE;
static u8 g_factory_ready = 0;      //NOTE: 这里用一个标志位来避免重复准备工厂，准备工厂的过程会遍历整个设备描述表，复杂度为 O(N)，因此只需执行一次即可。
static device_hook_fn_t g_device_hook = 0;//这里0是NULL，表示默认没有注册钩子函数，调用时会先判断是否为NULL再调用，避免空指针异常

static const u8 g_debruijn_idx_32[32] =
{
    0, 1, 28, 2, 29, 14, 24, 3,
    30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7,
    26, 12, 18, 6, 11, 5, 10, 9
};

static u8 _lowest_bit_index(u32 value)
{
    /* De Bruijn 算法: O(1) 计算单比特值在 0~31 的索引 */
    u32 lowest = value & (0u - value);
    return g_debruijn_idx_32[(lowest * 0x077CB531u) >> 27];
}

static int _factory_prepare(void) //TODO:未来可以优化，用container_of宏反推出结构体就行，不需要额外的开销
{
    u32 i;

    if (g_factory_ready)
    {
        return DEVICE_OK;
    }

    g_known_mask = DEV_NONE;
    for (i = 0; i < 32u; i++)
    {
        g_desc_by_bitpos[i] = 0;
    }

    for (i = 0; i < DEVICE_DESC_COUNT; i++)
    {
        DeviceType bit = g_device_descs[i].bit;
        u8 bit_idx;

        /* 每个设备必须且只能占用 1 个 bit。 */
        if ((bit == DEV_NONE) || ((bit & (bit - 1u)) != 0u))
        {
            return DEVICE_ERR_BAD_CONFIG;
        }

        bit_idx = _lowest_bit_index(bit);
        /* 禁止多个设备复用同一个 bit。 */
        if (g_desc_by_bitpos[bit_idx] != 0)
        {
            return DEVICE_ERR_BAD_CONFIG;
        }

        g_desc_by_bitpos[bit_idx] = &g_device_descs[i];
        g_known_mask |= bit;
    }

    g_factory_ready = 1;
    return DEVICE_OK;
}

void device_set_hook(device_hook_fn_t hook)
{
    g_device_hook = hook;
}

DeviceType device_all_mask(void)
{
    if (_factory_prepare() != DEVICE_OK)
    {
        return DEV_NONE;
    }
    return g_known_mask;
}

int device_create(DeviceType devices)
{
    u32 pending;
    int ret;

    ret = _factory_prepare();
    if (ret != DEVICE_OK)
    {
        return ret;
    }

    if (devices == DEV_NONE)
    {
        return DEVICE_ERR_EMPTY_MASK;
    }

    if ((devices & (~g_known_mask)) != 0u)
    {
        return DEVICE_ERR_UNSUPPORTED;
    }

    pending = devices;
    while (pending != 0u)
    {
        /*
         * 逐次取出最低位的 1:
         * bit = pending & -pending
         * 这样可在一次调用中初始化多个设备，且复杂度与“置 1 的位数”成正比。
         */
        u32 bit = pending & (0u - pending);
        u8 bit_idx = _lowest_bit_index(bit);
        const DeviceDesc *desc = g_desc_by_bitpos[bit_idx];

        if ((g_initialized_mask & bit) != 0u)
        {
            pending &= (pending - 1u);
            continue;
        }

        if ((desc == 0) || (desc->init == 0))
        {
            return DEVICE_ERR_UNSUPPORTED;
        }

        if (g_device_hook != 0)
        {
            /* 初始化前钩子 */
            g_device_hook(bit_idx, desc->name, bit, DEV_HOOK_BEFORE, DEVICE_OK);
        }

        ret = desc->init();

        if (g_device_hook != 0)
        {
            /* 初始化后钩子 */
            g_device_hook(bit_idx, desc->name, bit, DEV_HOOK_AFTER, ret);
        }

        if (ret != DEVICE_OK)
        {
            return ret;
        }

        g_initialized_mask |= bit;
        /* 清除刚处理完的最低位 1，继续处理下一个设备。 */
        pending &= (pending - 1u);
    }

    return DEVICE_OK;
}


void _device_init_hook(u8 idx, const char *name, DeviceType bit, DeviceHookStage stage, int ret)
{
    if (stage == DEV_HOOK_BEFORE)
    {
        log_i("dev_init start: idx=%u name=%s bit=0x%08lX",
              (unsigned int)idx,
              name,
              (unsigned long)bit);
    }
    else
    {
        if (ret == DEVICE_OK)
        {
            log_i("dev_init ok: idx=%u name=%s", (unsigned int)idx, name);
        }
        else
        {
            log_e("dev_init fail: idx=%u name=%s ret=%d", (unsigned int)idx, name, ret);
        }
    }
}
