#ifndef _LED_SMG_H
#define _LED_SMG_H
#include "sys.h"

// 74HC138操作线
#define LED_A0 PCout(10) //A0地址线
#define LED_A1 PCout(11) //A1地址线
#define LED_A2 PCout(12) //A2地址线

// 74HC595操作线
#define LED_DS		PBout(3) //串行数据输入
#define LED_LCLK	PBout(4) //锁存时钟输入
#define LED_SCK		PBout(5) //移位时钟输入

// 数码管段码(共阴)
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
#define SMG_SEG_DP  0x01   // 小数点

#define SMG_NUM     8      // 数码管个数

typedef struct {
    u8 buf[SMG_NUM];    // 显示缓冲区(段码)，下标0~7对应位0~7
    u8 current_bit;     // 当前扫描位(0~7)
    u8 ghost_flag;      // 消影标志(交替0/1)
} SMG_TypeDef;

extern SMG_TypeDef g_smg;

// 旧API(保留兼容)
void LED_SMG_Init(void);
void LED_Refresh(void);
void LED_Write_Data(u8 duan, u8 wei);

// 新API
u8   LED_SMG_SegOfNum(u8 num);           // 数字(0~9)转段码
void LED_SMG_WriteSeg(u8 bit, u8 seg);   // 写入段码到指定位()
void LED_SMG_WriteNum(u8 bit, u8 num);   // 写入数字(0~9)到指定位
void LED_SMG_WriteNumDP(u8 bit, u8 num); // 写入数字+小数点
void LED_SMG_WriteValue_contain_zero(u32 value, u8 start_bit, u8 len); // 写入多位数(不会消前导零)
void LED_SMG_WriteValue(u32 value, u8 start_bit, u8 len); // 写入多位数(自动消前导零)
void LED_SMG_Clear(void);                // 清除显示缓冲区
void LED_SMG_Scan(void);                 // 单次扫描(由定时器中断周期性调用)

#endif
