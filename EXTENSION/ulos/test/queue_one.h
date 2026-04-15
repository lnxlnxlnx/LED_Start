#ifndef F3643DEF_B7AF_4742_8D2A_604C4573D9E4
#define F3643DEF_B7AF_4742_8D2A_604C4573D9E4
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
    while(1) {
        ul_queue_send(queue, msg, sizeof(msg), UL_TRUE);
        ul_thread_delay(1000);
    }
}

void receiver_thread(void *p) {
    char buffer[32];
    while(1) {
        if (ul_queue_receive(queue, buffer, sizeof(buffer), 100) == UL_EOK) {
            // 处理接收到的消息
            printf("Received: %s\n", buffer);
        }
        ul_thread_delay(1000);
    }
}

void queue_test_start_thread_entry(void *p)
{
    ul_kprintf("=== Multi-thread Queue Test Start ===\r\n");
    queue = ul_queue_create("my_queue", 32, 10); // 创建一个队列，消息大小为32字节，容量为10条消息
    ul_kprintf("Queue created successfully\r\n");
    sender_thread_handle = ul_thread_create("sender", sender_thread, UL_NULL, 512, 1, 1);
    receiver_thread_handle = ul_thread_create("receiver", receiver_thread, UL_NULL, 512, 1, 1);
    ul_thread_startup(sender_thread_handle);
    ul_thread_startup(receiver_thread_handle);
    while(1) {
        ul_thread_delay(1000);
    }
}


#endif /* F3643DEF_B7AF_4742_8D2A_604C4573D9E4 */
