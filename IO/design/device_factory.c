#include "device_factory.h"
#include "led.h"
#include "exti.h"
#include "rtc.h"

static int _device_led_init(void)
{
    LED_Init();
    return DEVICE_OK;
}

static int _device_key_exti_init(void)
{
    EXTIX_Init();
    return DEVICE_OK;
}

static int _device_rtc_init(void)
{
    return RTC_Init() ? DEVICE_ERR_INIT_FAILED : DEVICE_OK;
}

static int _device_delay_init(void)
{
    delay_init();
    return DEVICE_OK;
}

static const device_init_fn_t g_device_init_by_bit[] =
{
    _device_led_init,
    _device_key_exti_init,
    _device_rtc_init,
    _device_delay_init
};

#define DEVICE_BIT_COUNT ((u32)(sizeof(g_device_init_by_bit) / sizeof(g_device_init_by_bit[0])))

static const u8 g_debruijn_idx_32[32] =
{
    0, 1, 28, 2, 29, 14, 24, 3,
    30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7,
    26, 12, 18, 6, 11, 5, 10, 9
};

static u8 _lowest_bit_index(u32 value)
{
    u32 lowest = value & (0u - value);
    return g_debruijn_idx_32[(lowest * 0x077CB531u) >> 27];
}

DeviceType device_all_mask(void)
{
    if (DEVICE_BIT_COUNT >= 32u)
    {
        return (DeviceType)0xFFFFFFFFu;
    }

    return (DeviceType)((1u << DEVICE_BIT_COUNT) - 1u);
}

int device_create(DeviceType devices)
{
    u32 pending;
    u32 known_mask = device_all_mask();
    static u32 initialized_mask = DEV_NONE;

    if (devices == DEV_NONE)
    {
        return DEVICE_ERR_EMPTY_MASK;
    }

    if ((devices & (~known_mask)) != 0u)
    {
        return DEVICE_ERR_UNSUPPORTED;
    }

    pending = devices;
    while (pending != 0u)
    {
        u32 bit = pending & (0u - pending);
        u8 idx = _lowest_bit_index(bit);
        device_init_fn_t init_fn;
        int ret;

        if ((initialized_mask & bit) != 0u)
        {
            pending &= (pending - 1u);
            continue;
        }

        if ((u32)idx >= DEVICE_BIT_COUNT)
        {
            return DEVICE_ERR_UNSUPPORTED;
        }

        init_fn = g_device_init_by_bit[idx];
        if (init_fn == 0)
        {
            return DEVICE_ERR_UNSUPPORTED;
        }

        ret = init_fn();
        if (ret != DEVICE_OK)
        {
            return ret;
        }

        initialized_mask |= bit;
        pending &= (pending - 1u);
    }

    return DEVICE_OK;
}
