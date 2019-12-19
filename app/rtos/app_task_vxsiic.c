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

#include "cmsis_os.h"
#include "app_tasks.h"
#include "app_task_vxsiic_impl.h"
#include "debug_helpers.h"

osThreadId vxsiicThreadId = NULL;
enum { vxsiicThreadStackSize = threadStackSize };
static const uint32_t vxsiicTaskLoopDelay = 10;

static void vxsiic_start_thread( void const *arg)
{
    (void) arg;

    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    task_vxsiic_init();

    while (1)
    {
        task_vxsiic_run();
        osDelay(vxsiicTaskLoopDelay);
    }
}

osThreadDef(vxsiic, vxsiic_start_thread,    osPriorityNormal, 1, vxsiicThreadStackSize);

void create_task_vxsiic(void)
{
    vxsiicThreadId = osThreadCreate(osThread (vxsiic), NULL);
    if (vxsiicThreadId == NULL) {
        debug_print("Failed to create vxsiic thread\n");
    }
}
