/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * license and copyright intentionally withheld to promote copying into user
 * code.
 */

#include "FreeRTOS.h"
#include "task.h"

#define TickTimer timer_0
#define TimerIntSource itc_src_tmr

/* Constants required to setup the task context. */
#define portINITIAL_SPSR                   ( ( StackType_t ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT                 ( ( StackType_t ) 0x20 )
#define portINSTRUCTION_SIZE               ( ( StackType_t ) 4 )
#define portNO_CRITICAL_SECTION_NESTING    ( ( StackType_t ) 0 )

// void vTaskDelay(const TickType_t xTicksToDelay) PRIVILEGED_FUNCTION;
// void vTaskStartScheduler(void) PRIVILEGED_FUNCTION;
//
// TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode, const char *const pcName,
// 	                const configSTACK_DEPTH_TYPE uxStackDepth,
// 	                void *const pvParameters, UBaseType_t uxPriority,
// 	                StackType_t *const puxStackBuffer,
// 	                StaticTask_t *const pxTaskBuffer) PRIVILEGED_FUNCTION;


// needed for the tick interrupt
static void prvSetupTimerInterrupt( void );
void vTickISR( void ) __attribute__( ( interrupt("IRQ") ) );

// TODO: ?
static void prvSetupTimerInterrupt( void ){
    itc_set_handler(TimerIntSource, vTickISR);
    itc_enable_interrupt(TimerIntSource);

    TimerInt_Init(TickTimer, configTICK_RATE_HZ);
}

BaseType_t xPortStartScheduler(void) { 
    // Iniciar el timer que genera las interrupciones.
    // Las interrupciones ya deberían de estar desactivadas aqui.
    prvSetupTimerInterrupt();

    vPortISRStartFirstTask();

    return pdTRUE; 
}

void vPortEndScheduler(void) {
    // Nothing to do, the scheduler shouldn't end
}

// TODO:Copied from LPC2000
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
	                                 TaskFunction_t pxCode, void *pvParameters) {
	(void)pxTopOfStack;
	(void)pvParameters;
	(void)*pxCode;

	StackType_t * pxOriginalTOS = pxTopOfStack;

    pxTopOfStack--;

    /* First on the stack is the return address - which in this case is the
     * start of the task.  The offset is added to make the return address appear
     * as it would within an IRQ ISR. */
    *pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;
    pxTopOfStack--;

    *pxTopOfStack = ( StackType_t ) 0xaaaaaaaa;    /* R14 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x12121212;    /* R12 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x11111111;    /* R11 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x10101010;    /* R10 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x09090909;    /* R9 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x08080808;    /* R8 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x07070707;    /* R7 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x06060606;    /* R6 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x05050505;    /* R5 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x04040404;    /* R4 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x03030303;    /* R3 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x02020202;    /* R2 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x01010101;    /* R1 */
    pxTopOfStack--;

    /* When the task starts is will expect to find the function parameter in
     * R0. */
    *pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
    pxTopOfStack--;

    /* The last thing onto the stack is the status register, which is set for
     * system mode, with interrupts enabled. */
    *pxTopOfStack = ( StackType_t ) portINITIAL_SPSR;

    if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00 )
    {
        /* We want the task to start in thumb mode. */
        *pxTopOfStack |= portTHUMB_MODE_BIT;
    }

    pxTopOfStack--;

    /* Some optimisation levels use the stack differently to others.  This
     * means the interrupt flags cannot always be stored on the stack and will
     * instead be stored in a variable, which is then saved as part of the
     * tasks context. */
    *pxTopOfStack = portNO_CRITICAL_SECTION_NESTING;


	return pxTopOfStack;
}

// void vPortYield(void) __atribute__(NAKED) {
//     /* Within an IRQ ISR the link register has an offset from the true return
//      * address, but an SWI ISR does not.  Add the offset manually so the same
//      * ISR return code can be used in both cases. */
//     __asm volatile ( "ADD       LR, LR, #4" );
//
//     /* Perform the context switch.  First save the context of the current task. */
//     portSAVE_CONTEXT();
//
// 	/* Switch to the highest priority task that is ready to run. */
// 	 vTaskSwitchContext();
//
//     /* Restore the context of the new task. */
//     portRESTORE_CONTEXT();
// }

// static void prvTickISR(void) {
//     /* Interrupts must have been enabled for the ISR to fire, so we have to
//     * save the context with interrupts enabled. */
//     /* Maintain the tick count. */
//     if (xTaskIncrementTick() != pdFALSE) {
//         /* Switch to the highest priority task that is ready to run. */
//         vTaskSwitchContext();
//     }
//     /* start executing the new task */
// }
