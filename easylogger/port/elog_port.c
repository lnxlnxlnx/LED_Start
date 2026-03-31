/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include "rtc.h"

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void)
{
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */

    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void)
{

    /* add your code here */
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size)
{
    /* output to terminal */
    printf("%.*s", size, log);
    /* add your code here */
}

/**
 * output lock
 */
void elog_port_output_lock(void)
{
    // 在这里加：关中断 / 拿互斥锁 / 你的RTOS锁机制

    // 方案A：最简单 → 关全局中断（所有RTOS都通用）
    __disable_irq();

    // 方案B：如果你自己写了互斥锁
    // your_mutex_lock(&elog_mutex);
    /* add your code here */
}

/**
 * output unlock
 */
void elog_port_output_unlock(void)
{
    // 对应解锁

    // 方案A：开中断
    __enable_irq();

    // 方案B：释放互斥锁
    // your_mutex_unlock(&elog_mutex);
    /* add your code here */
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void)
{
    // 定义静态缓冲区，用于存放格式化后的时间字符串
    static char time_buf[30];

    // 先获取一次最新时间
    RTC_Get();

    // 格式化输出：年-月-日 时:分:秒
    sprintf(time_buf, "%04d-%02d-%02d %02d:%02d:%02d",
            calendar.w_year,
            calendar.w_month,
            calendar.w_date,
            calendar.hour,
            calendar.min,
            calendar.sec);

    // 返回字符串指针
    return time_buf;
    /* add your code here */
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void)
{
    return "main"; // 这里简单返回一个字符串，表示当前进程/线程的名字，你可以根据你的RTOS获取实际的名字
    /* add your code here */
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void)
{
    return "main"; // 这里简单返回一个字符串，表示当前线程的名字，你可以根据你的RTOS获取实际的名字
    /* add your code here */
}