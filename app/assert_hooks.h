//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ASSERT_HOOKS_H
#define ASSERT_HOOKS_H

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void app_exit_handler(int result);

// FreeRTOS callbacks
extern void vAssertCalled( unsigned long ulLine, const uint8_t * const pcFileName );
extern void vApplicationMallocFailedHook( void );
//extern void vApplicationIdleHook( void );
extern void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
//extern void vApplicationTickHook( void );

#ifdef __cplusplus
}
#endif

#endif // ASSERT_HOOKS_H
