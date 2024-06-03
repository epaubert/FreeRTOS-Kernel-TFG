/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * license and copyright intentionally withheld to promote copying into user
 * code.
 */

#include "FreeRTOS.h"
#include "task.h"


/* Constants required to setup the task context. */
#define portINITIAL_SPSR                   ( ( StackType_t ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT                 ( ( StackType_t ) 0x20 )
#define portINSTRUCTION_SIZE               ( ( StackType_t ) 4 )
#define portNO_CRITICAL_SECTION_NESTING    ( ( StackType_t ) 0 )

// FIXME: ?
void vTaskDelay(const TickType_t xTicksToDelay) PRIVILEGED_FUNCTION;
// FIXME: ?
void vTaskStartScheduler(void) PRIVILEGED_FUNCTION;
TaskHandle_t
xTaskCreateStatic(TaskFunction_t pxTaskCode, const char *const pcName,
	                const configSTACK_DEPTH_TYPE uxStackDepth,
	                void *const pvParameters, UBaseType_t uxPriority,
	                StackType_t *const puxStackBuffer,
	                StaticTask_t *const pxTaskBuffer) PRIVILEGED_FUNCTION;

// FIXME:
BaseType_t xPortStartScheduler(void) { return pdTRUE; }

// FIXME: ?
void vPortEndScheduler(void) {}

// TODO:
// FIXME:
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

// FIXME:
void vPortYield(void) {
	/* Save the current Context */

	/* Switch to the highest priority task that is ready to run. */
#if (configNUMBER_OF_CORES == 1)
	{ vTaskSwitchContext(); }
#else
	{
	  vTaskSwitchContext(portGET_CORE_ID());
	}
#endif

	/* Start executing the task we have just switched to. */
}

// FIXME:?
static void prvTickISR(void) {
	/* Interrupts must have been enabled for the ISR to fire, so we have to
	 * save the context with interrupts enabled. */

#if (configNUMBER_OF_CORES == 1)
	{
	  /* Maintain the tick count. */
	  if (xTaskIncrementTick() != pdFALSE) {
	    /* Switch to the highest priority task that is ready to run. */
	    vTaskSwitchContext();
	  }
	}
#else
	{
	  UBaseType_t ulPreviousMask;

	  /* Tasks or ISRs running on other cores may still in critical section in
	   * multiple cores environment. Incrementing tick needs to performed in
	   * critical section. */
	  ulPreviousMask = taskENTER_CRITICAL_FROM_ISR();

	  /* Maintain the tick count. */
	  if (xTaskIncrementTick() != pdFALSE) {
	    /* Switch to the highest priority task that is ready to run. */
	    vTaskSwitchContext(portGET_CORE_ID());
	  }

	  taskEXIT_CRITICAL_FROM_ISR(ulPreviousMask);
	}
#endif /* if ( configNUMBER_OF_CORES == 1 ) */

	/* start executing the new task */
}
