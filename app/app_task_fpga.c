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
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "dev_fpga_types.h"
#include "dev_fpga.h"
#include "debug_helpers.h"

osThreadId fpgaThreadId = NULL;
enum { fpgaThreadStackSize = threadStackSize };
static const uint32_t fpgaTaskLoopDelay = 10;

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

typedef enum {
    FPGA_STATE_RESET,
    FPGA_STATE_RUN,
    FPGA_STATE_PAUSE,
    FPGA_STATE_ERROR,
} fpga_state_t;

static fpga_state_t state = FPGA_STATE_RESET;
static fpga_state_t old_state = FPGA_STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static void struct_fpga_init(Dev_fpga *d)
{
    d->present = DEVICE_UNKNOWN;
    d->id = 0;
}

static void fpga_task_init(void)
{
    struct_fpga_init(get_dev_fpga());
}

static void fpga_task_run(void)
{
    Dev_fpga *d = get_dev_fpga();
    old_state = state;
    switch (state) {
    case FPGA_STATE_RESET:
        if (DEVICE_NORMAL == fpgaDetect(d))
            state = FPGA_STATE_RUN;
        else
            state = FPGA_STATE_ERROR;
        break;
    case FPGA_STATE_RUN:
        if ((HAL_OK != fpgaWriteBmcVersion()) ||
                (HAL_OK != fpgaWriteBmcTemperature(get_dev_thset())) ||
                (HAL_OK != fpgaWritePllStatus(get_dev_pll())))
            state = FPGA_STATE_ERROR;
        state = FPGA_STATE_PAUSE;
        break;
    case FPGA_STATE_PAUSE:
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = FPGA_STATE_RUN;
        }
        break;
    case FPGA_STATE_ERROR:
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = FPGA_STATE_RESET;
        }
        break;
    default:
        break;
    }
    if (old_state != state) {
        stateStartTick = osKernelSysTick();
    }
}

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
