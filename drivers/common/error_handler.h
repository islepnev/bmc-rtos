/*
**    Copyright 2019 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) void app_exit_handler(int result);
__attribute__((noreturn)) void Error_Handler(void);

/*
#include "FreeRTOS.h"
#include "task.h"
// FreeRTOS callbacks
extern void vApplicationMallocFailedHook( void );
extern void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
*/

#ifdef __cplusplus
}
#endif

#endif // ERROR_HANDLER_H
