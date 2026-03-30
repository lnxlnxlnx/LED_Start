;/**
; * @addtogroup CORTEX-M23
; */
;/*@{*/

SCB_VTOR        EQU     0xE000ED08               ; Vector Table Offset Register
NVIC_INT_CTRL   EQU     0xE000ED04               ; interrupt control state register
NVIC_SHPR3    EQU     0xE000ED20               ; system priority register (2)
NVIC_PENDSV_PRI EQU     0xFFFF0000               ; PendSV and SysTick priority value (lowest)
NVIC_PENDSVSET  EQU     0x10000000               ; value to trigger PendSV exception

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

    IMPORT ul_thread_switch_interrupt_flag
    IMPORT ul_interrupt_from_thread_sp
    IMPORT ul_interrupt_to_thread_sp

;/**
; * @brief disable hardware interrupts
; *
; * @return current interrupt status
; */
ul_hw_interrupt_disable    PROC
    EXPORT  ul_hw_interrupt_disable
    MRS     r0, PRIMASK
    CPSID   I
    BX      LR
    ENDP

;/**
; * @brief enable hardware interrupts
; *
; * @param level interrupt status to restore
; */
ul_hw_interrupt_enable    PROC
    EXPORT  ul_hw_interrupt_enable
    MSR     PRIMASK, r0
    BX      LR
    ENDP

;/**
; * @brief switch context from one thread to another
; *
; * @param from source thread stack pointer
; * @param to destination thread stack pointer
; */
ul_hw_context_switch_interrupt
    EXPORT ul_hw_context_switch_interrupt
ul_hw_context_switch    PROC
    EXPORT ul_hw_context_switch

    ; set thread switch flag
    LDR     r2, =ul_thread_switch_interrupt_flag
    LDR     r3, [r2]
    CMP     r3, #1
    BEQ     _reswitch
    MOV     r3, #1
    STR     r3, [r2]

    ; set from thread
    LDR     r2, =ul_interrupt_from_thread_sp
    STR     r0, [r2]

_reswitch
    ; set to thread
    LDR     r2, =ul_interrupt_to_thread_sp
    STR     r1, [r2]

    ; trigger PendSV exception
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    BX      LR
    ENDP

;/**
; * @brief PendSV exception handler for context switching
; *
; * This handler performs the actual context switch between threads.
; * It saves the current thread's context and restores the new thread's context.
; */
PendSV_Handler   PROC
    EXPORT PendSV_Handler

    ; disable interrupts during context switch
    MRS     r2, PRIMASK
    CPSID   I

    ; check if context switch is needed
    LDR     r0, =ul_thread_switch_interrupt_flag
    LDR     r1, [r0]
    CMP     r1, #0x00
    BEQ     pendsv_exit

    ; clear switch flag
    MOV     r1, #0x00
    STR     r1, [r0]

    ; check if we need to save current context
    LDR     r0, =ul_interrupt_from_thread_sp
    LDR     r1, [r0]
    CMP     r1, #0x00
    BEQ     switch_to_thread

    ; save current context
    MRS     r1, psp
    SUBS    r1, r1, #0x20
    LDR     r0, [r0]
    STR     r1, [r0]

    STMIA   r1!, {r4 - r7}
    MOV     r4, r8
    MOV     r5, r9
    MOV     r6, r10
    MOV     r7, r11
    STMIA   r1!, {r4 - r7}

switch_to_thread
    ; restore new thread context
    LDR     r1, =ul_interrupt_to_thread_sp
    LDR     r1, [r1]
    LDR     r1, [r1]

    LDMIA   r1!, {r4 - r7}
    PUSH    {r4 - r7}

    LDMIA   r1!, {r4 - r7}
    MOV     r8,  r4
    MOV     r9,  r5
    MOV     r10, r6
    MOV     r11, r7

    POP     {r4 - r7}
    MSR     psp, r1

pendsv_exit
    ; restore interrupt state and return
    MSR     PRIMASK, r2
    MOVS    r0, #0x03
    RSBS    r0, r0, #0x00
    BX      r0

    ENDP

;/**
; * @brief perform the first thread switch
; *
; * @param to target thread stack pointer
; *
; * This function is used to perform the first thread switch when starting
; * the scheduler.
; */
ul_hw_context_switch_first    PROC
    EXPORT ul_hw_context_switch_first
    ; set target thread
    LDR     r1, =ul_interrupt_to_thread_sp
    STR     r0, [r1]

    ; set from thread to 0 (first switch)
    LDR     r1, =ul_interrupt_from_thread_sp
    MOV     r0, #0x0
    STR     r0, [r1]

    ; set switch flag
    LDR     r1, =ul_thread_switch_interrupt_flag
    MOV     r0, #1
    STR     r0, [r1]

    ; set PendSV priority
    LDR     r0, =NVIC_SHPR3
    LDR     r1, =NVIC_PENDSV_PRI
    LDR     r2, [r0,#0x00]
    ORRS    r1,r1,r2
    STR     r1, [r0]

    ; trigger PendSV
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    ; restore MSP
    LDR     r0, =SCB_VTOR
    LDR     r0, [r0]
    LDR     r0, [r0]
    MSR     msp, r0

    ; enable interrupts
    CPSIE   I

    ; ensure PendSV is processed
    DSB
    ISB

    ENDP

    ALIGN   4

    END
