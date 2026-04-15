/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-10     zhuqinsheng  the first version
 * 2025-12-17     zhuqinsheng  add ul_kprintf
 */
#ifndef _UL_DEBUG_H
#define _UL_DEBUG_H

#include "ul_config.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if (ULOS_CONFIG_USE_ASSERT == 1)
#define UL_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    ul_assert_handler(#EX, __FILE__, __LINE__);                           \
} 
#else
#define UL_ASSERT(x)   
#endif

void ul_kprintf(const char *fmt, ...);
void ul_assert_handler(const char *expr, const char *func, int line);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
