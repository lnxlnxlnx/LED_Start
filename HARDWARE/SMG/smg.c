/**
 * @file    smg.c
 * @brief   数码管(SMG)驱动实现
 *          驱动方式: 74HC138 位选 + 74HC595 段选，动态扫描刷新
 *          消影策略: 每次扫描先送空白段码，再切换位选，交替消影
 */

#include "smg.h"
#include "delay.h"
#include "timer.h"

SMG_TypeDef g_smg;  // 全局数码管实例，供外部访问显示缓冲区

/* 共阴段码表(0~9)，按数值索引即可获得对应段码 */
static const u8 g_seg_tab[] = {
    SMG_SEG_0, SMG_SEG_1, SMG_SEG_2, SMG_SEG_3, SMG_SEG_4,
    SMG_SEG_5, SMG_SEG_6, SMG_SEG_7, SMG_SEG_8, SMG_SEG_9
};

/**
 * @brief   初始化数码管模块 GPIO
 *          配置 PB3~5 (74HC595) 和 PC10~12 (74HC138) 为推挽输出
 *          开启必要时钟并释放 SWD 引脚 (PB3/PB4)
 */
void LED_SMG_Init(void)
{
    RCC->APB2ENR |= 1 << 3;    // 使能 PORTB 时钟
    RCC->APB2ENR |= 1 << 4;    // 使能 PORTC 时钟
    RCC->APB2ENR |= 1 << 0;    // 使能 AFIO 时钟 (用于 JTAG 重映射)

    JTAG_Set(SWD_ENABLE);      // 关闭 JTAG，释放 PB3/PB4 作为普通 IO

    GPIOB->CRL &= 0XFF000FFF;
    GPIOB->CRL |= 0X00333000;  // PB3(DS), PB4(LCLK), PB5(SCK) 推挽输出

    GPIOC->CRH &= 0XFFF000FF;
    GPIOC->CRH |= 0X00033300;  // PC10(A0), PC11(A1), PC12(A2) 推挽输出

    // 初始状态: 段选全部关闭，位选选中第 1 位 (便于消影逻辑衔接)
    GPIOB->BRR = 1 << 3 | 1 << 5;
    GPIOB->BSRR = 1 << 4;
    GPIOC->BSRR = 1 << 10 | 1 << 11 | 1 << 12;
}

/**
 * @brief   位选控制 (通过 74HC138 3-8 译码器选择当前显示的数码管位)
 * @param   num  位号 (0~7)
 */
static void LED_Wei(u8 num)
{
    LED_A0 = num & 0x01;
    LED_A1 = (num >> 1) & 0x01;
    LED_A2 = (num >> 2) & 0x01;
}

/**
 * @brief   通过 74HC595 写入段码并设置位选
 *          串行方式逐 bit 写入段码，然后切换 74HC138 选择位
 * @param   duan  段码 (8bit)
 * @param   wei   位号 (0~7)
 */
void LED_Write_Data(u8 duan, u8 wei)
{
    u8 i;
    for (i = 0; i < 8; i++) {
        LED_DS = (duan >> i) & 0x01;    // 段码最低位先送 (LSB first)
        LED_SCK = 0;                    // 移位时钟上升沿移入数据
        delay_us(5);
        LED_SCK = 1;
    }
    LED_Wei(wei);                       // 位选切换
}

/**
 * @brief   刷新锁存: 将 74HC595 移位寄存器数据锁存到输出引脚
 *          产生 LCLK 正脉冲完成锁存
 */
void LED_Refresh(void)
{
    LED_LCLK = 1;
    delay_us(5);
    LED_LCLK = 0;
}

/**
 * @brief   数字转共阴段码
 * @param   num  数字 0~9
 * @return  共阴段码，超出范围返回 0x00 (全灭)
 */
u8 LED_SMG_SegOfNum(u8 num)
{
    if (num <= 9) return g_seg_tab[num];
    return 0x00;
}

/**
 * @brief   将指定段码写入指定位的显示缓冲区
 * @param   bit  位号 (0~7)
 * @param   seg  段码
 */
void LED_SMG_WriteSeg(u8 bit, u8 seg)
{
    if (bit >= SMG_NUM) return;
    g_smg.buf[bit] = seg;
}

/**
 * @brief   将数字写入指定位
 * @param   bit  位号 (0~7)
 * @param   num  数字 (0~9)
 */
void LED_SMG_WriteNum(u8 bit, u8 num)
{
    LED_SMG_WriteSeg(bit, LED_SMG_SegOfNum(num));
}

/**
 * @brief   将数字 + 小数点写入指定位
 * @param   bit  位号 (0~7)
 * @param   num  数字 (0~9)
 */
void LED_SMG_WriteNumDP(u8 bit, u8 num)
{
    LED_SMG_WriteSeg(bit, LED_SMG_SegOfNum(num) | SMG_SEG_DP);
}

/**
 * @brief   清除所有显示缓冲区 (全部熄灭)
 */
void LED_SMG_Clear(void)
{
    u8 i;
    for (i = 0; i < SMG_NUM; i++)
        g_smg.buf[i] = 0x00;
}

/**
 * @brief   写入多位数到缓冲区 (保留前导零)
 *          例如 value=12, start_bit=0, len=5 显示 "00012"
 * @param   value     数值
 * @param   start_bit 起始位号
 * @param   len       位数
 */
void LED_SMG_WriteValue_contain_zero(u32 value, u8 start_bit, u8 len)
{
    u8 i;
    u32 divisor = 1;

    for (i = 1; i < len; i++) divisor *= 10;
    if (start_bit + len > SMG_NUM) len = SMG_NUM - start_bit;

    for (i = 0; i < len; i++) {
        u8 digit = (value / divisor) % 10;
        LED_SMG_WriteNum(start_bit + i, digit);
        divisor /= 10;
    }
}

/**
 * @brief   写入多位数到缓冲区 (自动消除前导零)
 *          例如 value=12, start_bit=0, len=5 显示 "   12" 而非 "00012"
 *          最低位无论是否为 0 都会显示 (保证至少显示一位)
 * @param   value     数值
 * @param   start_bit 起始位号
 * @param   len       位数
 */
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
            LED_SMG_WriteSeg(start_bit + i, 0x00);  // 前导零熄灭
        else
            LED_SMG_WriteNum(start_bit + i, digit);

        divisor /= 10;
    }
}

/**
 * @brief   数码管动态扫描 (由定时器中断周期性调用)
 *          采用"交替消影"策略:
 *            - 第 1 次: 送空白段码 + 位选 (消除上一帧残影)
 *            - 第 2 次: 送正常段码 + 位选 + 刷新锁存
 *          每次调用只处理一位，完整一轮需 2 * SMG_NUM 次调用
 * @note    建议每 2ms 调用一次, 完整刷新率 ≈ 1000/(2*8*2) ≈ 31Hz
 */
void LED_SMG_Scan(void)
{
    // static u8 t = 0;
    // if (++t < TIMER_MS(&g_tim4, 2)) return; // 每 2ms 执行一次扫描
    // t = 0;
    u8 bit = g_smg.current_bit;

    if (g_smg.ghost_flag) {
        /* 消影阶段: 先送空白段码到当前位，清除上一帧残影 */
        LED_Write_Data(0x00, bit);
    }
    else {
        /* 显示阶段: 送正常段码并锁存输出 */
        LED_Write_Data(g_smg.buf[bit], bit);
    }
    LED_Refresh();

    g_smg.ghost_flag ^= 1;  // 翻转消影标志

    /* 偶数次 (ghost_flag == 0) 表示显示阶段完成，切换到下一位 */
    if (!g_smg.ghost_flag) {
        g_smg.current_bit++;
        if (g_smg.current_bit >= SMG_NUM)
            g_smg.current_bit = 0;
    }
}
