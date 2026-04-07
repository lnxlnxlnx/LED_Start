#ifndef ACAFAC35_7869_4E4F_A39A_34EA2ADA19C5
#define ACAFAC35_7869_4E4F_A39A_34EA2ADA19C5
#include "ul_thread.h"
#include "ul_ipc.h"
#include "sys.h"
#include "usart.h"
#include "ul_debug.h"

ul_thread_t *sender_thread_handle;
ul_thread_t *receiver_thread_handle;
ul_queue_t *queue;

void sender_thread(void *p) {
    char msg[] = "Hello";
    u32 count = 0;
    
    ul_kprintf("[Sender] Thread started\r\n");
    
    while(1) {
        ul_ecode ret = ul_queue_send(queue, msg, sizeof(msg), UL_TRUE);  // ✅ 使用阻塞模式
        
        if (ret == UL_EOK) {
            ul_kprintf("[Sender] Send #%d: %s\r\n", ++count, msg);
        } else {
            ul_kprintf("[Sender] Send failed: %d\r\n", ret);
        }
        
        ul_thread_delay(1000);
    }
}

void receiver_thread(void *p) {
    char buffer[32];
    u32 count = 0;
    
    ul_kprintf("[Receiver] Thread started\r\n");
    
    while(1) {
        // ✅ 超时时间设置为大于发送间隔
        if (ul_queue_receive(queue, buffer, sizeof(buffer), 1500) == UL_EOK) {
            ul_kprintf("[Receiver] Recv #%d: %s\r\n", ++count, buffer);
        }
        // ✅ 移除或缩短 delay，让接收更及时
        // ul_thread_delay(100);  // 可选：短 delay 避免空转
    }
}

void queue_test_start_thread_entry(void *p)
{
    ul_kprintf("=== Multi-thread Queue Test Start ===\r\n");
    
    // ✅ 确保队列容量足够
    queue = ul_queue_create("my_queue", 32, 10);
    
    if (queue == UL_NULL) {
        ul_kprintf("[ERROR] Queue create failed!\r\n");
        return;
    }
    
    ul_kprintf("Queue created successfully\r\n");
    
    sender_thread_handle = ul_thread_create("sender", sender_thread, UL_NULL, 512, 1, 1);
    receiver_thread_handle = ul_thread_create("receiver", receiver_thread, UL_NULL, 512, 1, 1);
    
    ul_thread_startup(sender_thread_handle);
    ul_thread_startup(receiver_thread_handle);
    
    while(1) {
        ul_thread_delay(1000);
    }
}

#endif 
