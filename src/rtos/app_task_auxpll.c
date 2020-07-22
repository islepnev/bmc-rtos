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

#include "app_task_auxpll.h"

#include <stdint.h>

#include "cmsis_os.h"
#include "app_tasks.h"
#include "ad9516/app_task_auxpll_impl.h"
#include "debug_helpers.h"

osThreadId auxpllThreadId = NULL;
enum { auxpllThreadStackSize = threadStackSize };
static const uint32_t auxpllTaskLoopDelay = 10;

static void auxpllTask(void const *arg)
{
    (void) arg;
    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    while(1) {
        auxpll_task_run();
        osDelay(auxpllTaskLoopDelay);
    }
}

osThreadDef(auxpll, auxpllTask, osPriorityNormal,      1, auxpllThreadStackSize);

void create_task_auxpll(void)
{
    auxpllThreadId = osThreadCreate(osThread (auxpll), NULL);
    if (auxpllThreadId == NULL) {
        debug_print("Failed to create auxpll thread\n");
    }
}
