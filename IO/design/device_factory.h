#ifndef D085E887_BC54_433A_A988_3D85ED04BC29
#define D085E887_BC54_433A_A988_3D85ED04BC29

#include "sys.h"

/*
 * DeviceType 使用位掩码表示设备集合，可通过按位或一次初始化多个设备。
 * 例如: DEV_LED | DEV_RTC
 */
typedef u32 DeviceType;
typedef int (*device_init_fn_t)(void);

typedef enum
{
    DEV_HOOK_BEFORE = 0,
    DEV_HOOK_AFTER
} DeviceHookStage;

typedef void (*device_hook_fn_t)(u8 idx, const char *name, DeviceType bit, DeviceHookStage stage, int ret);

#define DEVICE_OK                  0
#define DEVICE_ERR_UNSUPPORTED    -1
#define DEVICE_ERR_INIT_FAILED    -2
#define DEVICE_ERR_EMPTY_MASK     -3
#define DEVICE_ERR_BAD_CONFIG     -4

#define DEV_NONE                    ((DeviceType)0u)
/*
 * DEV_DELAY 放在 bit0，用于保证在“最低位优先初始化”策略下最先执行。
 */
#define DEV_DELAY                   ((DeviceType)(1u << 0))
#define DEV_LED                     ((DeviceType)(1u << 1))
#define DEV_KEY_EXTI                ((DeviceType)(1u << 2))
#define DEV_RTC                     ((DeviceType)(1u << 3))

#define DEV_GROUP_IO                (DEV_LED | DEV_KEY_EXTI)
#define DEV_GROUP_TIME              (DEV_RTC | DEV_DELAY)
#define DEV_GROUP_BOOT              (DEV_GROUP_IO | DEV_GROUP_TIME)

void device_set_hook(device_hook_fn_t hook);
DeviceType device_all_mask(void);
int device_create(DeviceType devices);

/* 示例 hook，可直接注册到 device_set_hook() 使用。 */
void _device_init_hook(u8 idx, const char *name, DeviceType bit, DeviceHookStage stage, int ret);

#endif /* D085E887_BC54_433A_A988_3D85ED04BC29 */
