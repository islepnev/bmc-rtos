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
#include "dev_leds.h"
#include "app_tasks.h"
#include "app_shared_data.h"
#include "dev_fpga.h"
#include "debug_helpers.h"
#include "logbuffer.h"
#include "bsp.h"
#include "bsp_pin_defs.h"

osThreadId fpgaThreadId = NULL;
enum { fpgaThreadStackSize = threadStackSize };
static const uint32_t fpgaTaskLoopDelay = 10;

static const uint32_t LOAD_DELAY_TICKS = 5000;
static const uint32_t DETECT_DELAY_TICKS = 1000;
static const uint32_t ERROR_DELAY_TICKS = 1000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

typedef enum {
    FPGA_STATE_STANDBY,
    FPGA_STATE_LOAD,
    FPGA_STATE_RESET,
    FPGA_STATE_RUN,
    FPGA_STATE_PAUSE,
    FPGA_STATE_ERROR,
} fpga_state_t;

static fpga_state_t state = FPGA_STATE_STANDBY;
static fpga_state_t old_state = FPGA_STATE_STANDBY;

static uint32_t fpga_load_start_tick = 0;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static void struct_fpga_init(Dev_fpga *d)
{
    Dev_fpga zz = {0};
    *d = zz;
}

static void fpga_task_init(void)
{
    struct_fpga_init(&dev.fpga);
}

static void fpga_task_run(void)
{
    Dev_fpga *d = &dev.fpga;
    old_state = state;
    if (board_version >= PCB_4_2) {
        d->initb = HAL_GPIO_ReadPin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin);
        d->done = HAL_GPIO_ReadPin(FPGA_DONE_GPIO_Port, FPGA_DONE_Pin);
    } else {
        d->initb = 1;
        d->done = 1;
    }
   int fpga_core_power_present = get_fpga_core_power_present(&dev.pm);
    int fpga_power_present = enable_power && fpga_core_power_present;
    int fpga_enable = fpga_power_present && d->initb;
//    int fpga_loading = fpga_power_present && d->initb && !d->done;
    int fpga_done = fpga_power_present && d->done;
    switch (state) {
    case FPGA_STATE_STANDBY:
        if (fpga_enable) {
            state = FPGA_STATE_LOAD;
            fpga_load_start_tick = osKernelSysTick();
        }
        d->present = DEVICE_UNKNOWN;
        break;
    case FPGA_STATE_LOAD:
        if (!fpga_enable)
            state = FPGA_STATE_STANDBY;
        if (fpga_done) {
            if (board_version >= PCB_4_2) {
                const uint32_t tick_freq_hz = 1000U / HAL_GetTickFreq();
                const uint32_t ticks = osKernelSysTick() - fpga_load_start_tick;
                log_printf(LOG_INFO, "FPGA loaded in %u ms", ticks * 1000 / tick_freq_hz);
            }
            state = FPGA_STATE_RESET;
        }
        if (stateTicks() > LOAD_DELAY_TICKS) {
            log_put(LOG_ERR, "FPGA load timeout");
            state = FPGA_STATE_ERROR;
        }
        break;
    case FPGA_STATE_RESET:
        if (!fpga_power_present)
            state = FPGA_STATE_STANDBY;
        if (1
                && DEVICE_NORMAL == fpgaDetect(d)
                && DEVICE_NORMAL == fpga_test(d)
                )
            state = FPGA_STATE_RUN;
        if (stateTicks() > DETECT_DELAY_TICKS) {
            log_put(LOG_ERR, "FPGA detect timeout");
            state = FPGA_STATE_ERROR;
        }
        break;
    case FPGA_STATE_RUN:
        if (!fpga_power_present)
            state = FPGA_STATE_STANDBY;
        if ((DEVICE_NORMAL != fpga_check_live_magic(d)) ||
                (HAL_OK != fpgaWriteBmcVersion()) ||
                (HAL_OK != fpgaWriteBmcTemperature(&dev.thset)) ||
                (HAL_OK != fpgaWritePllStatus(&dev.pll)) ||
                (HAL_OK != fpgaWriteSystemStatus(&dev)) ||
                (HAL_OK != fpgaWriteSensors(&dev.pm))
                ) {
            state = FPGA_STATE_ERROR;
            break;
        }
        state = FPGA_STATE_PAUSE;
        break;
    case FPGA_STATE_PAUSE:
        if (!fpga_done)
            state = FPGA_STATE_STANDBY;
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = FPGA_STATE_RUN;
        }
        break;
    case FPGA_STATE_ERROR:
        d->present = DEVICE_FAIL;
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = FPGA_STATE_STANDBY;
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

//    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

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
