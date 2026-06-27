/**
 * @file    smg.h
 * @brief   数码管(SMG)驱动模块 — 74HC138(位选) + 74HC595(段选)
 *          支持 8 位共阴数码管，段码表内置，带消影扫描
 * @note    旧 API (LED_Write_Data / LED_Refresh) 保留兼容，新代码优先使用新 API
 */

#ifndef _LED_SMG_H
#define _LED_SMG_H
#include "sys.h"

/* ======================== 硬件引脚定义 ======================== */

// 74HC138 位选控制线 (3-8 译码器, 输出选通某一位)
#define LED_A0 PCout(10)   // A0 地址线
#define LED_A1 PCout(11)   // A1 地址线
#define LED_A2 PCout(12)   // A2 地址线

// 74HC595 段选控制线 (串入并出移位寄存器)
#define LED_DS   PBout(3)  // 串行数据输入 (Data Serial)
#define LED_LCLK PBout(4)  // 锁存时钟 (Latch Clock)
#define LED_SCK  PBout(5)  // 移位时钟 (Shift Clock)

/* ======================== 数码管段码(共阴) ======================== */
#define SMG_SEG_0   0xfc
#define SMG_SEG_1   0x60
#define SMG_SEG_2   0xda
#define SMG_SEG_3   0xf2
#define SMG_SEG_4   0x66
#define SMG_SEG_5   0xb6
#define SMG_SEG_6   0xbe
#define SMG_SEG_7   0xe0
#define SMG_SEG_8   0xfe
#define SMG_SEG_9   0xf6
#define SMG_SEG_DP  0x01   // 小数点段 (DP)

/* ======================== 数码管数量 ======================== */
#define SMG_NUM     8      // 板上数码管个数

/* ======================== 数码管控制结构体 ======================== */
typedef struct {
    u8 buf[SMG_NUM];        // 显示缓冲区(段码)，下标 0~7 对应位 0~7
    u8 current_bit;         // 当前正在扫描的位 (0~7)
    u8 ghost_flag;          // 消影交替标志，每次扫描翻转，偶数次更新位号
} SMG_TypeDef;

extern SMG_TypeDef g_smg;   // 全局数码管实例

/* ======================== 函数声明 ======================== */

/**
 * @brief   初始化数码管 GPIO (PB3~5, PC10~12 推挽输出)
 *          开启 PORTB/PORTC/AFIO 时钟，释放 SWD 的 PB3/PB4
 */
void LED_SMG_Init(void);

/**
 * @brief   刷新锁存：将 74HC595 移位寄存器的数据锁存到输出
 * @note    旧 API，保留兼容
 */
void LED_Refresh(void);

/**
 * @brief   向 74HC595 写入段码 + 74HC138 位选
 * @param   duan  段码 (8bit, 含小数点)
 * @param   wei   位选编号 (0~7)
 * @note    旧 API，保留兼容
 */
void LED_Write_Data(u8 duan, u8 wei);

/**
 * @brief   数字(0~9)转共阴段码
 * @param   num  数字 0~9
 * @return  对应的共阴段码，超出范围返回 0x00
 */
u8   LED_SMG_SegOfNum(u8 num);

/**
 * @brief   将指定段码写入指定位的缓冲区
 * @param   bit  位号 (0~7)
 * @param   seg  段码值
 */
void LED_SMG_WriteSeg(u8 bit, u8 seg);

/**
 * @brief   将数字(0~9)写入指定位
 * @param   bit  位号 (0~7)
 * @param   num  数字 0~9
 */
void LED_SMG_WriteNum(u8 bit, u8 num);

/**
 * @brief   将数字(0~9) + 小数点写入指定位
 * @param   bit  位号 (0~7)
 * @param   num  数字 0~9
 */
void LED_SMG_WriteNumDP(u8 bit, u8 num);

/**
 * @brief   写入多位数到缓冲区的指定位置 (不消除前导零)
 * @param   value     待显示的数值
 * @param   start_bit 起始位号
 * @param   len       位数 (不超过 (SMG_NUM - start_bit))
 */
void LED_SMG_WriteValue_contain_zero(u32 value, u8 start_bit, u8 len);

/**
 * @brief   写入多位数到缓冲区的指定位置 (自动消除前导零)
 * @param   value     待显示的数值
 * @param   start_bit 起始位号
 * @param   len       位数 (不超过 (SMG_NUM - start_bit))
 * @note    例如 value=12, len=5 会显示 "   12" 而非 "00012"
 */
void LED_SMG_WriteValue(u32 value, u8 start_bit, u8 len);

/**
 * @brief   清除所有数码管显示缓冲区
 */
void LED_SMG_Clear(void);

/**
 * @brief   单次数码管扫描 (应由定时器中断周期性调用)
 *          每次扫描一位，采用交替消影法消除鬼影
 *          建议扫描周期: 每 2ms 扫描一位，完整一轮约 16ms (62.5Hz 刷新率)
 */
void LED_SMG_Scan(void);
void LED_SMG_AutoCycle(void);

#endif
