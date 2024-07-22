#include "FreeRTOS.h"
#include "portmacro.h"

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING    ( ( uint32_t ) 2 )
volatile uint32_t ulCriticalNesting = 9999UL;

/* ISR to handle manual context switches (from a call to taskYIELD()). */
// void vPortYieldProcessor( void ) __attribute__( (naked) );
// void vPortYieldProcessor( void ) __attribute__( (interrupt("SWI")) );

/*
 * The scheduler can only be started from ARM mode, hence the inclusion of this
 * function here.
 */
void vPortISRStartFirstTask( void );
/*-----------------------------------------------------------*/
// void vTickISR( void ) __attribute__( ( naked ) );
// void vTickISR( void ) __attribute__( ( interrupt("IRQ") ) );

void vPortISRStartFirstTask( void )
{
    /* Simply start the scheduler.  This is included here as it can only be
     * called from ARM mode. */
    portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 *
 * When a context switch is performed from the task level the saved task
 * context is made to look as if it occurred from within the tick ISR.  This
 * way the same restore context function can be used when restoring the context
 * saved from the ISR or that saved from a call to vPortYieldProcessor.
 */
__attribute__( (interrupt("SWI")) ) void vPortYieldProcessor( void )
{
    /* Within an IRQ ISR the link register has an offset from the true return
     * address, but an SWI ISR does not.  Add the offset manually so the same
     * ISR return code can be used in both cases. */
    __asm volatile ( "ADD       LR, LR, #4" );

    /* Perform the context switch.  First save the context of the current task. */
    portSAVE_CONTEXT();

    /* Find the highest priority task that is ready to run. */
    // __asm volatile ( "bl vTaskSwitchContext" );
    vTaskSwitchContext();

    /* Restore the context of the new task. */
    portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/
/*
 * The ISR used for the scheduler tick.
 */

extern BaseType_t xTaskIncrementTick( void );

__attribute__( ( interrupt("IRQ") ) ) void vTickISR( void )
{
    /* Save the context of the interrupted task. */
    portSAVE_CONTEXT();

    /* Increment the RTOS tick count, then look for the highest priority
     * task that is ready to run. */

    if(xTaskIncrementTick() == pdTRUE){
        vTaskSwitchContext();
    }

    // __asm volatile
    // (
    //     "   bl xTaskIncrementTick   \t\n" \
    //     "   cmp r0, #0              \t\n" \
    //     "   beq SkipContextSwitch   \t\n" \
    //     "   bl vTaskSwitchContext   \t\n" \
    //     "SkipContextSwitch:         \t\n"
    // );

    /* Ready for the next interrupt. */
    clearInt(getIntTmr());
    // __asm volatile(
    //     "   bl      getIntTmr          \t\n" \
    //     "   bl      clearInt           \t\n"
    // );

    /* Restore the context of the new task. */
    portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

/* The code generated by the GCC compiler uses the stack in different ways at
 * different optimisation levels.  The interrupt flags can therefore not always
 * be saved to the stack.  Instead the critical section nesting level is stored
 * in a variable, which is then saved as part of the stack context. */
void vPortEnterCritical( void )
{
    /* Disable interrupts as per portDISABLE_INTERRUPTS();                          */
    __asm volatile (
        "STMDB  SP!, {R0}           \n\t" /* Push R0.                             */
        "MRS    R0, CPSR            \n\t" /* Get CPSR.                            */
        "ORR    R0, R0, #0xC0       \n\t" /* Disable IRQ, FIQ.                    */
        "MSR    CPSR, R0            \n\t" /* Write back modified value.           */
        "LDMIA  SP!, {R0}" );             /* Pop R0.                              */

    /* Now that interrupts are disabled, ulCriticalNesting can be accessed
     * directly.  Increment ulCriticalNesting to keep a count of how many times
     * portENTER_CRITICAL() has been called. */
    ulCriticalNesting++;
}

void vPortExitCritical( void )
{
    if( ulCriticalNesting > portNO_CRITICAL_NESTING )
    {
        /* Decrement the nesting count as we are leaving a critical section. */
        ulCriticalNesting--;

        /* If the nesting level has reached zero then interrupts should be
         * re-enabled. */
        if( ulCriticalNesting == portNO_CRITICAL_NESTING )
        {
            /* Enable interrupts as per portEXIT_CRITICAL().                    */
            __asm volatile (
                "STMDB  SP!, {R0}       \n\t" /* Push R0.                     */
                "MRS    R0, CPSR        \n\t" /* Get CPSR.                    */
                "BIC    R0, R0, #0xC0   \n\t" /* Enable IRQ, FIQ.             */
                "MSR    CPSR, R0        \n\t" /* Write back modified value.   */
                "LDMIA  SP!, {R0}" );         /* Pop R0.                      */
        }
    }
}
