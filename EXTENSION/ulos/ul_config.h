#ifndef _UL_CONFIG_H
#define _UL_CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ul_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* System Configuration */
#define ULOS_CONFIG_MAX_PRIORITY      ( 32 )
#define ULOS_MAX_TICK                 ( 0xFFFFFFFFFFFFFFFFUL )
#define ULOS_MAX_DELAY                ULOS_MAX_TICK

/* Feature Switches */
#define ULOS_CONFIG_USE_TOPIC           ( 0 )
#define ULOS_CONFIG_USE_MUTEX           ( 1 )
#define ULOS_CONFIG_USE_QUEUE           ( 1 )
#define ULOS_CONFIG_USE_SEMAPHORE       ( 1 )
#define ULOS_CONFIG_USE_EVENT           ( 1 )
#define ULOS_CONFIG_USE_TIMER           ( 1 )
#define ULOS_CONFIG_USE_KERNEL_PRINTF   ( 1 ) // if (1) please see ul_debug.c

/* Timer Configuration */
#define ULOS_CONFIG_TIMER_THREAD_PRIORITY   ( ULOS_CONFIG_MAX_PRIORITY - 2 )
#define ULOS_CONFIG_TIMER_THREAD_STACK_SIZE ( 512 )

/* Hook Configuration */
#define ULOS_CONFIG_USE_IDLEHOOK      ( 0 )
#define ULOS_CONFIG_USE_TICKHOOK      ( 0 )

/* Scheduler Configuration */
#define ULOS_CONFIG_SCHED_ALG_FFS     ( 1 )

/* Memory Configuration */
#define UL_HEAP_SIZE                  ((ul_size_t)(1024 * 10))

/* Assert Configuration */
#define ULOS_CONFIG_USE_ASSERT       ( 0 )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UL_CONFIG_H */
