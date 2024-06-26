/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * license and copyright intentionally withheld to promote copying into user
 * code.
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */
/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

// System definitions from the bsp
#include "system.h"

/* Type definitions. */
#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE portLONG

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
#define portPOINTER_SIZE_TYPE size_t

#if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
    typedef uint16_t     TickType_t;
    #define portMAX_DELAY    ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
    typedef uint32_t     TickType_t;
    #define portMAX_DELAY    ( TickType_t ) ( 0xFFFFFFFFUL )
#else
    #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

/* Architecture specifics. */
#define portSTACK_GROWTH (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT 4
#define portNOP() __asm volatile("NOP");

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

/* Check the configuration. */
#if (configMAX_PRIORITIES > 32)
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
#endif

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY(uxPriority, uxReadyPriorities)               \
  (uxReadyPriorities) |= (1UL << (uxPriority))
#define portRESET_READY_PRIORITY(uxPriority, uxReadyPriorities)                \
  (uxReadyPriorities) &= ~(1UL << (uxPriority))

#define portRESTORE_CONTEXT()                                                 \
    {                                                                         \
        extern volatile void * volatile pxCurrentTCB;                         \
        extern volatile uint32_t ulCriticalNesting;                           \
                                                                              \
        /* Set the LR to the task stack. */                                   \
        __asm volatile (                                                      \
            "LDR        R0, =pxCurrentTCB                               \n\t" \
            "LDR        R0, [R0]                                        \n\t" \
            "LDR        LR, [R0]                                        \n\t" \
                                                                              \
            /* The critical nesting depth is the first item on the stack. */  \
            /* Load it into the ulCriticalNesting variable. */                \
            "LDR        R0, =ulCriticalNesting                          \n\t" \
            "LDMFD  LR!, {R1}                                           \n\t" \
            "STR        R1, [R0]                                        \n\t" \
                                                                              \
            /* Get the SPSR from the stack. */                                \
            "LDMFD  LR!, {R0}                                           \n\t" \
            "MSR        SPSR, R0                                        \n\t" \
                                                                              \
            /* Restore all system mode registers for the task. */             \
            "LDMFD  LR, {R0-R14}^                                       \n\t" \
            "NOP                                                        \n\t" \
                                                                              \
            /* Restore the return address. */                                 \
            "LDR        LR, [LR, #+60]                                  \n\t" \
                                                                              \
            /* And return - correcting the offset in the LR to obtain the */  \
            /* correct address. */                                            \
            "SUBS   PC, LR, #4                                          \n\t" \
            );                                                                \
        ( void ) ulCriticalNesting;                                           \
        ( void ) pxCurrentTCB;                                                \
    }
/*-----------------------------------------------------------*/

#define portSAVE_CONTEXT()                                                    \
    {                                                                         \
        extern volatile void * volatile pxCurrentTCB;                         \
        extern volatile uint32_t ulCriticalNesting;                           \
                                                                              \
        /* Push R0 as we are going to use the register. */                    \
        __asm volatile (                                                      \
            "STMDB  SP!, {R0}                                           \n\t" \
                                                                              \
            /* Set R0 to point to the task stack pointer. */                  \
            "STMDB  SP,{SP}^                                            \n\t" \
            "NOP                                                        \n\t" \
            "SUB    SP, SP, #4                                          \n\t" \
            "LDMIA  SP!,{R0}                                            \n\t" \
                                                                              \
            /* Push the return address onto the stack. */                     \
            "STMDB  R0!, {LR}                                           \n\t" \
                                                                              \
            /* Now we have saved LR we can use it instead of R0. */           \
            "MOV    LR, R0                                              \n\t" \
                                                                              \
            /* Pop R0 so we can save it onto the system mode stack. */        \
            "LDMIA  SP!, {R0}                                           \n\t" \
                                                                              \
            /* Push all the system mode registers onto the task stack. */     \
            "STMDB  LR,{R0-LR}^                                         \n\t" \
            "NOP                                                        \n\t" \
            "SUB    LR, LR, #60                                         \n\t" \
                                                                              \
            /* Push the SPSR onto the task stack. */                          \
            "MRS    R0, SPSR                                            \n\t" \
            "STMDB  LR!, {R0}                                           \n\t" \
                                                                              \
            "LDR    R0, =ulCriticalNesting                              \n\t" \
            "LDR    R0, [R0]                                            \n\t" \
            "STMDB  LR!, {R0}                                           \n\t" \
                                                                              \
            /* Store the new top of stack for the task. */                    \
            "LDR    R0, =pxCurrentTCB                                   \n\t" \
            "LDR    R0, [R0]                                            \n\t" \
            "STR    LR, [R0]                                            \n\t" \
            );                                                                \
        ( void ) ulCriticalNesting;                                           \
        ( void ) pxCurrentTCB;                                                \
    }

extern void vTaskSwitchContext( void );
#define portYIELD_FROM_ISR()    vTaskSwitchContext()
#define portYIELD()             __asm volatile ( "SWI 0" )
/*-----------------------------------------------------------*/

#define portGET_HIGHEST_PRIORITY(uxTopPriority, uxReadyPriorities)             \
  do {                                                                         \
    uxTopPriority = 0;                                                         \
  } while (0)

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* Disable the interrupts */
#define portDISABLE_INTERRUPTS()                                       \
__asm volatile (                                                       \
    "STMDB  SP!, {R0}       \n\t"   /* Push R0.                     */ \
    "MRS    R0, CPSR        \n\t"   /* Get CPSR.                    */ \
    "ORR    R0, R0, #0xC0   \n\t"   /* Disable IRQ, FIQ.            */ \
    "MSR    CPSR, R0        \n\t"   /* Write back modified value.   */ \
    "LDMIA  SP!, {R0}           " ) /* Pop R0.                      */

/* Enable the interrupts */
#define portENABLE_INTERRUPTS()                                        \
__asm volatile (                                                       \
    "STMDB  SP!, {R0}       \n\t"   /* Push R0.                     */ \
    "MRS    R0, CPSR        \n\t"   /* Get CPSR.                    */ \
    "BIC    R0, R0, #0xC0   \n\t"   /* Enable IRQ, FIQ.             */ \
    "MSR    CPSR, R0        \n\t"   /* Write back modified value.   */ \
    "LDMIA  SP!, {R0}           " ) /* Pop R0.                      */

#if (configNUMBER_OF_CORES == 1)
/* preserve current interrupt state and then disable interrupts */
#define portENTER_CRITICAL() itc_disable_ints()

/* restore previously preserved interrupt state */
#define portEXIT_CRITICAL() itc_restore_ints()

#else

/* The port can maintain the critical nesting count in TCB or maintain the
 * critical nesting count in the port. */
#define portCRITICAL_NESTING_IN_TCB 1

/* vTaskEnterCritical and vTaskExitCritical should be used in the implementation
 * of portENTER/EXIT_CRITICAL if the number of cores is more than 1 in the
 * system. */
#define portENTER_CRITICAL vTaskEnterCritical
#define portEXIT_CRITICAL vTaskExitCritical

/* vTaskEnterCriticalFromISR and vTaskExitCriticalFromISR should be used in the
 * implementation of portENTER/EXIT_CRITICAL_FROM_ISR if the number of cores is
 * more than 1 in the system. */
#define portENTER_CRITICAL_FROM_ISR vTaskEnterCriticalFromISR
#define portEXIT_CRITICAL_FROM_ISR vTaskExitCriticalFromISR

#endif /* if ( configNUMBER_OF_CORES == 1 ) */

// extern void vPortYield(void);
// #define portYIELD() vPortYield()

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters)                       \
  void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)                             \
  void vFunction(void *pvParameters)

#endif /* PORTMACRO_H */
