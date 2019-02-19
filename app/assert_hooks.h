#ifndef ASSERT_HOOKS_H
#define ASSERT_HOOKS_H

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
extern void vApplicationMallocFailedHook( void );
extern void vApplicationIdleHook( void );
extern void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
extern void vApplicationTickHook( void );

#ifdef __cplusplus
}
#endif

#endif // ASSERT_HOOKS_H
