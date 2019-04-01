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

#include "app_task_vxsiic.h"

#include <stdio.h>
#include "cmsis_os.h"
#include "app_tasks.h"
#include "app_task_vxsiic_impl.h"
#include "debug_helpers.h"

enum { vxsiicThreadStackSize = threadStackSize };
static const uint32_t vxsiicTaskLoopDelay = 10;

static void vxsiic_thread_task( void const *arg)
{
    (void) arg;

    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

    while (1)
    {
        vxsiic_task();
        osDelay(vxsiicTaskLoopDelay);
    }
}

osThreadDef(vxsiic, vxsiic_thread_task,    osPriorityLow, 1, vxsiicThreadStackSize);

void create_task_vxsiic(void)
{
    powermonThreadId = osThreadCreate(osThread (vxsiic), NULL);
    if (powermonThreadId == NULL) {
        debug_printf("Failed to create vxsiic thread\n");
    }
}
