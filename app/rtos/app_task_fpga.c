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

#include "app_task_fpga.h"

#include <stdint.h>
#include "cmsis_os.h"
#include "app_tasks.h"
#include "debug_helpers.h"
#include "app_task_fpga_impl.h"

osThreadId fpgaThreadId = NULL;
enum { fpgaThreadStackSize = threadStackSize };
static const uint32_t fpgaTaskLoopDelay = 10;

static void start_fpga_thread(void const *arg)
{
    (void) arg;

    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

    fpga_task_init();
    for( ;; )
    {
        fpga_task_run();
        osDelay(fpgaTaskLoopDelay);
    }
}

osThreadDef(fpga, start_fpga_thread, osPriorityLow,  1, fpgaThreadStackSize);

void create_task_fpga(void)
{
        fpgaThreadId = osThreadCreate(osThread (fpga), NULL);
        if (fpgaThreadId == NULL) {
            debug_print("Failed to create fpga thread\n");
        }
}
