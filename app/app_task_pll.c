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

#include "app_task_pll.h"

#include <stdint.h>

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "app_tasks.h"
#include "app_shared_data.h"
#include "dev_pll.h"
#include "debug_helpers.h"
#include "pll_i2c_driver.h"

osThreadId pllThreadId = NULL;
enum { pllThreadStackSize = threadStackSize };
static const uint32_t pllTaskLoopDelay = 10;

static void pllTask(void const *arg)
{
    (void) arg;
//    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    pll_i2c_init();
    while(1) {
        pllRun(&dev.pll);
        osDelay(pllTaskLoopDelay);
    }
}

osThreadDef(pll, pllTask, osPriorityAboveNormal, 1, pllThreadStackSize);

void create_task_pll(void)
{
    pllThreadId = osThreadCreate(osThread (pll), NULL);
    if (pllThreadId == NULL) {
        debug_print("Failed to create pll thread\n");
    }
}
