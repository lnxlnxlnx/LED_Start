/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-7-21     zhuqinsheng   the first version
 * 2025-8-13     zhuqinsheng   修改了ul_button_t的一些属性, 主要增加了type成员
 * 2025-12-9     zhuqinsheng   优化了初始化机制,新增长按循环触发功能
 */
#ifndef _UL_BUTTON_H
#define _UL_BUTTON_H

#include "ul_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 按键事件枚举 */
typedef enum
{
    UL_BUTTON_EVENT_PRESSED        = 0x01,    /* 单击事件 */
    UL_BUTTON_EVENT_LONGPRESS      = 0x02,    /* 长按事件 */
    UL_BUTTON_EVENT_DOUBLE_CLICK   = 0x03,    /* 双击事件 */
} button_event_e;

/* 对象类型声明 */
typedef struct ul_button ul_button_t;

/* 回调函数类型定义 */
typedef void (*ul_button_event_cb_func)(button_event_e event);

/* 移植接口 */
/**
 * @brief 读取button状态
 * @param self 按键对象指针
 * @return 0 按钮未被按下
 * @return 1 按钮已被按下
 * @note 用户实现此接口时其返回值必须为按下时为1,松开时为0
 */
typedef ul_uint32_t (*ul_button_read_fuc)(void);

/* 默认时间配置 */
#define UL_BUTTON_CONFIG_LP_TIME    (800)    /* 长按时间阈值(ms) */
#define UL_BUTTON_CONFIG_DP_TIME  (300)    /* 双击时间阈值(ms) */

/* 按键接口函数声明 */
/**
 * @brief 初始化一个静态的button对象
 * @param self button对象的地址
 * @param read_pin 读取按键状态的函数指针
 * @param cb_func 按键事件回调函数指针
 * @return 错误代码
 */
ul_ecode ul_button_init(ul_button_t *self,
                               ul_button_read_fuc read_pin,
                               ul_button_event_cb_func cb_func);

/**
 * @brief 从系统中分离button对象
 * @param self button对象的地址
 * @return 错误代码
 */
ul_ecode ul_button_detach(ul_button_t *self);

/**
 * @brief 动态创建一个button对象
 * @param read_pin 读取按键状态的函数指针
 * @param cb_func 按键事件回调函数指针
 * @return button对象的地址
 */
ul_button_t* ul_button_create(ul_button_read_fuc read_pin,
                                     ul_button_event_cb_func cb_func);

/**
 * @brief 删除一个动态的button对象
 * @param self button对象的地址
 * @return 错误代码
 */
ul_ecode ul_button_delete(ul_button_t *self);

/**
 * @brief 设置button长按和双击的时间阈值
 * @param self button对象的地址
 * @param lp_time 长按时间阈值(ms)
 * @param dp_time 双击时间阈值(ms)
 */
void ul_button_presstime_change(ul_button_t *self, 
                               ul_uint32_t lp_time, 
                               ul_uint32_t dp_time);

/**
 * @brief 注册button事件回调函数
 * @param self button对象的地址
 * @param func 要注册的事件回调函数地址
 */
void ul_button_register_callback(ul_button_t *self, 
                                ul_button_event_cb_func func);

/**
 * @brief button定时器回调函数
 * @param period 调用周期(ms)
 * @note 需以一个固定周期调用此函数
 */
void ul_button_timer_callback(ul_uint32_t period);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UL_BUTTON_H__ */
