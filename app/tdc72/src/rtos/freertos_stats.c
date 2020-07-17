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
#include "freertos_stats.h"

#include "FreeRTOS.h"
#include "task.h"

// FreeRTOSConfig.h should define:
//   configGENERATE_RUN_TIME_STATS = 1
//   configSUPPORT_DYNAMIC_ALLOCATION = 1
//   configUSE_TRACE_FACILITY = 1

static uint32_t save_TotalTime = 0;
static uint32_t save_RunTimeCounter = 0;

uint32_t freertos_get_cpu_load_percent(void)
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalTime;
    uint32_t result = 0;

    const TaskHandle_t idleTaskHandle = xTaskGetIdleTaskHandle();
    if (idleTaskHandle == NULL)
        return 0;
    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof( TaskStatus_t ));

    if (pxTaskStatusArray == NULL)
        return 0;

    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalTime );

    if (ulTotalTime == 0) goto free_return;
    for (x = 0; x < uxArraySize; x++) {
        if (idleTaskHandle != pxTaskStatusArray[ x ].xHandle)
            continue;
        uint32_t ulRunTimeCounter = pxTaskStatusArray[ x ].ulRunTimeCounter;
        uint32_t total_time = ulTotalTime - save_TotalTime;
        uint32_t idle_time = ulRunTimeCounter - save_RunTimeCounter;
        if (total_time > 0)
            result = (uint32_t)(total_time - idle_time) / (total_time / 100U);
        save_TotalTime = ulTotalTime;
        save_RunTimeCounter = ulRunTimeCounter;
    }
free_return:
    vPortFree( pxTaskStatusArray );
    return result;
}
