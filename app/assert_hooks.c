#include "assert_hooks.h"

#include <FreeRTOS.h>
#include <task.h>

/*
void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{

//  static portBASE_TYPE xPrinted = pdFALSE;
  volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

  taskDISABLE_INTERRUPTS(); // game over

  printf("\nError: assert called at %s:%ld\n", pcFileName, ulLine);

    taskENTER_CRITICAL();
    {
        // You can step out of this function to debug the assertion by using
        // the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
        // value
        while( ulSetToNonZeroInDebuggerToContinue == 0 )
        {
        }
    }
    taskEXIT_CRITICAL();
}

void vApplicationStackOverflowHook( xTaskHandle *pxTsk, signed portCHAR *pcTskNm )
{

  taskDISABLE_INTERRUPTS(); // game over

  printf("\nError: stack overflow in task %s\n", pcTskNm);
  for( ;; );
}

void vApplicationMallocFailedHook( void )
{

  taskDISABLE_INTERRUPTS(); // game over

  printf("\nError: malloc failed\n");

  for( ;; );
}
*/

void vAssertCalled( uint32_t ulLine, const char *pcFile )
{
volatile unsigned long ul = 0;

    ( void ) pcFile;
    ( void ) ulLine;

    taskDISABLE_INTERRUPTS(); // game over

    printf("\nError: assert called at %s:%ld\n", pcFile, ulLine);


    taskENTER_CRITICAL();
    {
        /* Set ul to a non-zero value using the debugger to step out of this
        function. */
        while( ul == 0 )
        {
            __NOP();
        }
    }
    taskEXIT_CRITICAL();
}

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS(); // game over

    printf("\nError: malloc failed\n");

    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    taskDISABLE_INTERRUPTS(); // game over

    printf("\nError: stack overflow in task %s\n", pcTaskName);
    for( ;; );
}

void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace;

    /* This is just a trivial example of an idle hook.  It is called on each
    cycle of the idle task.  It must *NOT* attempt to block.  In this case the
    idle task just queries the amount of FreeRTOS heap that remains.  See the
    memory management section on the http://www.FreeRTOS.org web site for memory
    management options.  If there is a lot of heap memory free then the
    configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
    RAM. */
    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}
