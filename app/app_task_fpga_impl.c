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

#include "app_task_fpga_impl.h"

#include <stdint.h>
#include "cmsis_os.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "dev_fpga_types.h"
#include "dev_fpga.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "debug_helpers.h"
#include "logbuffer.h"

#include "fpga_spi_hal.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;

typedef enum {
    FPGA_STATE_STANDBY,
    FPGA_STATE_LOAD,
    FPGA_STATE_RUN,
    FPGA_STATE_PAUSE,
    FPGA_STATE_ERROR,
} fpga_state_t;

static fpga_state_t state = FPGA_STATE_STANDBY;
static fpga_state_t old_state = FPGA_STATE_STANDBY;

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

void fpga_task_init(void)
{
    struct_fpga_init(get_dev_fpga());
    fpgaInit();
}

static int old_enable_power = 0;

void fpga_task_run(void)
{
    if (old_enable_power != enable_power) {
        old_enable_power = enable_power;
        fpga_enable_interface(enable_power);
    }
    Dev_fpga *d = get_dev_fpga();
    d->initb = HAL_GPIO_ReadPin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin);
    d->done = HAL_GPIO_ReadPin(FPGA_DONE_GPIO_Port, FPGA_DONE_Pin);
    int fpga_enable = enable_power && d->initb;
    int fpga_loading = enable_power && d->initb && !d->done;
    int fpga_done = fpga_enable && d->done;
    switch (state) {
    case FPGA_STATE_STANDBY:
        if (fpga_loading)
            state = FPGA_STATE_LOAD;
        d->present = DEVICE_UNKNOWN;
        break;
    case FPGA_STATE_LOAD:
        if (!fpga_enable)
            state = FPGA_STATE_STANDBY;
        if (fpga_done)
            state = FPGA_STATE_RUN;
        break;
    case FPGA_STATE_RUN:
        if (!fpga_done)
            state = FPGA_STATE_STANDBY;
        if ((DEVICE_NORMAL != fpgaDetect(d)) ||
                (DEVICE_NORMAL != fpgaWriteBmcVersion(d)) ||
                (DEVICE_NORMAL != fpgaWriteBmcTemperature(d, get_dev_thset())) ||
                (DEVICE_NORMAL != fpgaWritePllStatus(d, get_dev_pll())))
            state = FPGA_STATE_ERROR;
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
        if (old_state != state) {
            log_printf(LOG_ERR, "FPGA SPI error");
        }
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = FPGA_STATE_RUN;
        }
        break;
    default:
        break;
    }
    if (old_state != state) {
        stateStartTick = osKernelSysTick();
    }
    old_state = state;
}
