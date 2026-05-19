#include "smg.h"
#include "delay.h"

SMG_TypeDef g_smg;  // 全局数码管实例

// 共阴段码表(0~9)
static const u8 g_seg_tab[] = {
    SMG_SEG_0, SMG_SEG_1, SMG_SEG_2, SMG_SEG_3, SMG_SEG_4,
    SMG_SEG_5, SMG_SEG_6, SMG_SEG_7, SMG_SEG_8, SMG_SEG_9
};

void LED_SMG_Init(void)
{
    RCC->APB2ENR |= 1 << 3;    // PORTB时钟
    RCC->APB2ENR |= 1 << 4;    // PORTC时钟
    RCC->APB2ENR |= 1 << 0;    // AFIO时钟

    JTAG_Set(SWD_ENABLE);      // PB3/4做普通IO

    GPIOB->CRL &= 0XFF000FFF;
    GPIOB->CRL |= 0X00333000;  // PB3~5推挽输出

    GPIOC->CRH &= 0XFFF000FF;
    GPIOC->CRH |= 0X00033300;  // PC10~12推挽输出

    GPIOB->BRR  = 1 << 3 | 1 << 5;
    GPIOB->BSRR = 1 << 4;
    GPIOC->BSRR = 1 << 10 | 1 << 11 | 1 << 12;
}

// 位选(74HC138)
static void LED_Wei(u8 num)
{
    LED_A0 = num & 0x01;
    LED_A1 = (num >> 1) & 0x01;
    LED_A2 = (num >> 2) & 0x01;
}

void LED_Write_Data(u8 duan, u8 wei)
{
    u8 i;
    for (i = 0; i < 8; i++) {
        LED_DS = (duan >> i) & 0x01;
        LED_SCK = 0;
        delay_us(5);
        LED_SCK = 1;
    }
    LED_Wei(wei);
}

void LED_Refresh(void)
{
    LED_LCLK = 1;
    delay_us(5);
    LED_LCLK = 0;
}

u8 LED_SMG_SegOfNum(u8 num)
{
    if (num <= 9) return g_seg_tab[num];
    return 0x00;
}

void LED_SMG_WriteSeg(u8 bit, u8 seg)
{
    if (bit >= SMG_NUM) return;
    g_smg.buf[bit] = seg;
}

void LED_SMG_WriteNum(u8 bit, u8 num)
{
    LED_SMG_WriteSeg(bit, LED_SMG_SegOfNum(num));
}

void LED_SMG_WriteNumDP(u8 bit, u8 num)
{
    LED_SMG_WriteSeg(bit, LED_SMG_SegOfNum(num) | SMG_SEG_DP);
}

void LED_SMG_Clear(void)
{
    u8 i;
    for (i = 0; i < SMG_NUM; i++)
        g_smg.buf[i] = 0x00;
}

void LED_SMG_WriteValue(u32 value, u8 start_bit, u8 len)
{
    u8 i;
    u32 divisor = 1;
    u8 leading = 1;

    for (i = 1; i < len; i++) divisor *= 10;
    if (start_bit + len > SMG_NUM) len = SMG_NUM - start_bit;

    for (i = 0; i < len; i++) {
        u8 digit = (value / divisor) % 10;

        if (digit != 0 || i == len - 1)
            leading = 0;

        if (leading)
            LED_SMG_WriteSeg(start_bit + i, 0x00);
        else
            LED_SMG_WriteNum(start_bit + i, digit);

        divisor /= 10;
    }
}

void LED_SMG_Scan(void)
{
    u8 bit = g_smg.current_bit;

    if (g_smg.ghost_flag) {
        LED_Write_Data(0x00, bit);  // 消影: 先送空白
    } else {
        LED_Write_Data(g_smg.buf[bit], bit);  // 正常显示
    }
    LED_Refresh();

    g_smg.ghost_flag ^= 1;  // 翻转消影标志

    if (!g_smg.ghost_flag) {
        g_smg.current_bit++;
        if (g_smg.current_bit >= SMG_NUM)
            g_smg.current_bit = 0;
    }
}
