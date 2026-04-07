#include "ul_config.h"

/**
 * @brief stack frame structure for context switching
 */
struct stack_frame
{
    /* r4 ~ r11 callee saved registers */
    ul_uint32_t r4;
    ul_uint32_t r5;
    ul_uint32_t r6;
    ul_uint32_t r7;
    ul_uint32_t r8;
    ul_uint32_t r9;
    ul_uint32_t r10;
    ul_uint32_t r11;

    struct {
        /* exception stack frame */
        ul_uint32_t r0;     /* argument register */
        ul_uint32_t r1;
        ul_uint32_t r2;
        ul_uint32_t r3;
        ul_uint32_t r12;
        ul_uint32_t lr;     /* return address */
        ul_uint32_t pc;     /* program counter */
        ul_uint32_t psr;    /* program status register */
    } exception_stack_frame;
};

/* interrupt handling flags */
ul_uint32_t ul_interrupt_from_thread_sp;   /* stack pointer of thread being switched from */
ul_uint32_t ul_interrupt_to_thread_sp;     /* stack pointer of thread being switched to */
ul_uint32_t ul_thread_switch_interrupt_flag; /* flag indicating thread switch request */

/**
 * @brief initialize thread stack
 *
 * @param tentry thread entry function
 * @param parameter thread parameter
 * @param stack_addr stack memory address
 * @param texit thread exit function
 *
 * @return initialized stack pointer
 *
 * This function initializes a thread's stack with proper context frame.
 * The stack layout follows ARM Cortex-M exception stack frame convention.
 */
ul_uint8_t *ul_hw_stack_init(void       *tentry,
                             void       *parameter,
                             ul_uint8_t *stack_addr,
                             void       *texit)
{
    struct stack_frame *stack_frame;
    ul_uint8_t         *stk;
    unsigned long       i;

    /* align stack to 8-byte boundary */
    stk  = stack_addr;
    stk  = (ul_uint8_t *)UL_ALIGN_DOWN((ul_uint32_t)stk, 8);
    stk -= sizeof(struct stack_frame);

    stack_frame = (struct stack_frame *)stk;

    /* initialize all registers with known pattern */
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(ul_uint32_t); i ++)
    {
        ((ul_uint32_t *)stack_frame)[i] = 0xdeadbeef;
    }

    /* initialize exception stack frame */
    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter; /* argument */
    stack_frame->exception_stack_frame.r1  = 0;                        /* r1 */
    stack_frame->exception_stack_frame.r2  = 0;                        /* r2 */
    stack_frame->exception_stack_frame.r3  = 0;                        /* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                        /* r12 */
    stack_frame->exception_stack_frame.lr  = (unsigned long)texit;     /* return address */
    stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;    /* entry point */
    stack_frame->exception_stack_frame.psr = 0x01000000L;              /* PSR (T-bit set) */
    
    /* 
     * Stack layout after initialization (assuming stack grows downward):
     * 
     * High Address
     * +-------------------+ <- stack_start + stack_size (initial SP)
     * |   xPSR            |
     * +-------------------+
     * |   PC (entry)      |
     * +-------------------+
     * |   LR (_thread_exit)|
     * +-------------------+
     * |   R12             |
     * +-------------------+
     * |   R3              |
     * +-------------------+
     * |   R2              |
     * +-------------------+
     * |   R1 (parameter)  |
     * +-------------------+
     * |   R0              |
     * +-------------------+
     * |   R11             |
     * +-------------------+
     * |   R10             |
     * +-------------------+
     * |   R9              |
     * +-------------------+
     * |   R8              |
     * +-------------------+
     * |   R7              |
     * +-------------------+
     * |   R6              |
     * +-------------------+
     * |   R5              |
     * +-------------------+
     * |   R4              |
     * +-------------------+ <- stack_top
     * |   ...             |
     * +-------------------+
     * |   ...             |
     * +-------------------+
     * |   ###...###       | <- unused space filled with pattern
     * +-------------------+
     * |                   |
     * +-------------------+
     * Low Address
     * ^                   
     * |                   
     * stack_start        
     */
    return stk;
}
