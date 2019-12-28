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

#include "app_task_pll.h"

#include <stdint.h>

#include "cmsis_os.h"
#include "app_tasks.h"
#include "app_task_eeprom_config_impl.h"
#include "app_task_clkmux_impl.h"
#include "app_task_pll_impl.h"
#include "debug_helpers.h"

osThreadId pllThreadId = NULL;
enum { pllThreadStackSize = threadStackSize };
static const uint32_t pllTaskLoopDelay = 50;

static void pllTask(void const *arg)
{
    (void) arg;
    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    task_clkmux_init();
    pll_task_init();
    while(1) {
        task_eeprom_config_run();
        task_clkmux_run();
        pll_task_run();
        osDelay(pllTaskLoopDelay);
    }
}

osThreadDef(pll, pllTask, osPriorityBelowNormal,      1, pllThreadStackSize);

void create_task_pll(void)
{
    pllThreadId = osThreadCreate(osThread (pll), NULL);
    if (pllThreadId == NULL) {
        debug_print("Failed to create pll thread\n");
    }
}
