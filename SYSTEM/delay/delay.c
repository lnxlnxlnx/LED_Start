#include "delay.h"

/*
 * SysTick 作为全局 1ms 心跳:
 * - delay_init() 只负责配置一次 1ms 中断
 * - SysTick_Handler() 中调用 delay_systick_handler() 完成计数
 * - delay_us()/delay_ms() 不再修改 SysTick 配置，避免破坏节拍统计
 */

static volatile u32 g_tick_ms = 0u;
static volatile u32 g_tick_s = 0u;
static u32 g_systick_reload = 0u;
static u32 g_cycles_per_us = 0u;
static volatile u8 g_delay_inited = 0u;

void delay_init(void)
{
    if (g_delay_inited != 0u)
    {
        return;
    }

    SystemCoreClockUpdate();

    g_cycles_per_us = SystemCoreClock / 1000000u;
    if (g_cycles_per_us == 0u)
    {
        g_cycles_per_us = 1u;
    }

    if (SysTick_Config(SystemCoreClock / 1000u) != 0u)
    {
        while (1)
        {
        }
    }

    g_systick_reload = SysTick->LOAD + 1u;
    g_delay_inited = 1u;
}

void delay_systick_handler(void)
{
    static u16 sub_ms = 0u;

    g_tick_ms++;
    sub_ms++;

    if (sub_ms >= 1000u)
    {
        sub_ms = 0u;
        g_tick_s++;
    }
}

u32 delay_get_ms(void)
{
    return g_tick_ms;
}

u32 delay_get_s(void)
{
    return g_tick_s;
}

void delay_ms(u16 nms)
{
    u32 start;

    if (g_delay_inited == 0u)
    {
        delay_init();
    }

    start = delay_get_ms();
    while ((delay_get_ms() - start) < (u32)nms)
    {
    }
}

void delay_us(u32 nus)
{
    u32 total_cycles;
    u32 elapsed_cycles = 0u;
    u32 prev_val;

    if (g_delay_inited == 0u)
    {
        delay_init();
    }

    total_cycles = nus * g_cycles_per_us;
    prev_val = SysTick->VAL;

    while (elapsed_cycles < total_cycles)
    {
        u32 now_val = SysTick->VAL;

        if (now_val != prev_val)
        {
            if (prev_val > now_val)
            {
                elapsed_cycles += (prev_val - now_val);
            }
            else
            {
                elapsed_cycles += (prev_val + (g_systick_reload - now_val));
            }
            prev_val = now_val;
        }
    }
}
