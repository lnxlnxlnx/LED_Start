#ifndef D085E887_BC54_433A_A988_3D85ED04BC29
#define D085E887_BC54_433A_A988_3D85ED04BC29

#include "sys.h"

typedef int (*device_init_fn_t)(void);

#define DEVICE_OK                  0
#define DEVICE_ERR_UNSUPPORTED    -1
#define DEVICE_ERR_INIT_FAILED    -2
#define DEVICE_ERR_EMPTY_MASK     -3

typedef u32 DeviceType;

#define DEV_NONE                    ((DeviceType)0u)
#define DEV_LED                     ((DeviceType)(1u << 0))
#define DEV_KEY_EXTI                ((DeviceType)(1u << 1))
#define DEV_RTC                     ((DeviceType)(1u << 2))

#define DEV_GROUP_IO                (DEV_LED | DEV_KEY_EXTI)
#define DEV_GROUP_TIME              (DEV_RTC)
#define DEV_GROUP_BOOT              (DEV_GROUP_IO | DEV_GROUP_TIME)
#define DEV_GROUP_ALL               (DEV_GROUP_BOOT)

int device_create(DeviceType devices);

#endif /* D085E887_BC54_433A_A988_3D85ED04BC29 */
