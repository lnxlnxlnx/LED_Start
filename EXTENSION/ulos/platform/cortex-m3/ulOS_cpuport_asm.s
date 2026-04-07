;/**
; * @addtogroup CORTEX-M3
; * modify from RT-Thread
; * @{
; */

SCB_VTOR        EQU     0xE000ED08               ; Vector Table Offset Register
NVIC_INT_CTRL   EQU     0xE000ED04               ; interrupt control state register
NVIC_SYSPRI2    EQU     0xE000ED20               ; system priority register (2)
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
    MRS     r0, PRIMASK              ; read current interrupt status
    CPSID   I                        ; disable interrupts
    BX      LR                       ; return
    ENDP

;/**
; * @brief enable hardware interrupts
; *
; * @param level interrupt status to restore
; */
ul_hw_interrupt_enable    PROC
    EXPORT  ul_hw_interrupt_enable
    MSR     PRIMASK, r0              ; restore interrupt status
    BX      LR                       ; return
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
    LDR     r2, =ul_thread_switch_interrupt_flag  ; load switch flag address
    LDR     r3, [r2]                               ; read current flag value
    CMP     r3, #1                                 ; check if flag is already set
    BEQ     _reswitch                              ; skip if already set
    MOV     r3, #1                                 ; set flag value
    STR     r3, [r2]                               ; store flag value

    ; set from thread
    LDR     r2, =ul_interrupt_from_thread_sp      ; load from thread pointer address
    STR     r0, [r2]                               ; store current thread pointer

_reswitch
    ; set to thread
    LDR     r2, =ul_interrupt_to_thread_sp        ; load to thread pointer address
    STR     r1, [r2]                               ; store target thread pointer

    ; trigger PendSV exception
    LDR     r0, =NVIC_INT_CTRL                     ; load interrupt control register
    LDR     r1, =NVIC_PENDSVSET                    ; load PendSV trigger value
    STR     r1, [r0]                               ; trigger PendSV

    BX      LR                                     ; return
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
    MRS     r2, PRIMASK              ; save current interrupt state
    CPSID   I                        ; disable interrupts

    ; check if context switch is needed
    LDR     r0, =ul_thread_switch_interrupt_flag  ; load switch flag address
    LDR     r1, [r0]                               ; read flag value
    CBZ     r1, pendsv_exit         ; exit if no switch needed

    ; clear switch flag
    MOV     r1, #0x00               ; clear flag value
    STR     r1, [r0]                ; store cleared flag

    ; check if we need to save current context
    LDR     r0, =ul_interrupt_from_thread_sp  ; load from thread address
    LDR     r1, [r0]                        ; read from thread pointer
    CBZ     r1, switch_to_thread    ; skip save if first switch

    ; save current context
    MRS     r1, psp                 ; get current stack pointer
    STMFD   r1!, {r4 - r11}         ; save registers r4-r11
    LDR     r0, [r0]                ; get thread control block
    STR     r1, [r0]                ; update stack pointer

switch_to_thread
    ; prepare to switch to new thread
    LDR     r1, =ul_interrupt_to_thread_sp  ; load to thread address
    LDR     r1, [r1]                        ; read to thread pointer
    LDR     r1, [r1]                        ; get target stack pointer

    ; restore new thread context
    LDMFD   r1!, {r4 - r11}         ; restore registers r4-r11
    MSR     psp, r1                 ; update stack pointer

pendsv_exit
    ; restore interrupt state
    MSR     PRIMASK, r2             ; restore saved interrupt state

    ; set return state
    ORR     lr, lr, #0x04           ; set EXC_RETURN to use PSP
    BX      lr                      ; return to new thread

    ENDP

;/**
; * @brief perform the first thread switch
; *
; * @param to target thread stack pointer
; *
; * This function is used to perform the first thread switch when starting
; * the scheduler. It initializes the necessary registers and triggers the
; * first context switch.
; */
ul_hw_context_switch_first    PROC
    EXPORT ul_hw_context_switch_first
    ; set target thread
    LDR     r1, =ul_interrupt_to_thread_sp    ; load to thread address
    STR     r0, [r1]                       ; store target thread pointer

    ; set from thread to 0 (first switch)
    LDR     r1, =ul_interrupt_from_thread_sp  ; load from thread address
    MOV     r0, #0x0                       ; clear r0
    STR     r0, [r1]                       ; store zero

    ; set switch flag
    LDR     r1, =ul_thread_switch_interrupt_flag  ; load flag address
    MOV     r0, #1                              ; set flag value
    STR     r0, [r1]                             ; store flag

    ; set PendSV priority
    LDR     r0, =NVIC_SYSPRI2               ; load priority register
    LDR     r1, =NVIC_PENDSV_PRI           ; load lowest priority value
    LDR.W   r2, [r0,#0x00]                 ; read current value
    ORR     r1,r1,r2                       ; combine with current value
    STR     r1, [r0]                       ; store new value

    ; trigger PendSV
    LDR     r0, =NVIC_INT_CTRL             ; load interrupt control
    LDR     r1, =NVIC_PENDSVSET            ; load trigger value
    STR     r1, [r0]                       ; trigger PendSV

    ; restore MSP
    LDR     r0, =SCB_VTOR                  ; load vector table address
    LDR     r0, [r0]                       ; read vector table
    LDR     r0, [r0]                       ; read initial SP
    MSR     msp, r0                        ; set MSP

    ; enable interrupts
    CPSIE   F                              ; enable fault exceptions
    CPSIE   I                              ; enable IRQ interrupts

    ; ensure PendSV is processed
    DSB                                     ; data synchronization barrier
    ISB                                     ; instruction synchronization barrier

    ENDP

    ALIGN   4

    END
