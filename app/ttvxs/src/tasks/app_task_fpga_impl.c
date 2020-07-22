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
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "fpga/dev_fpga_types.h"
#include "fpga/dev_fpga.h"
#include "fpga_spi_hal.h"
#include "powermon/dev_powermon.h"
#include "bsp.h"
#include "bsp_fpga.h"
#include "bsp_pin_defs.h"
#include "debug_helpers.h"
#include "logbuffer.h"

static const uint32_t LOAD_DELAY_TICKS = 5000;
static const uint32_t DETECT_DELAY_TICKS = 100;
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
    Dev_fpga zz = {};
    *d = zz;
    d->present = DEVICE_UNKNOWN;
}

void fpga_task_init(void)
{
    struct_fpga_init(get_dev_fpga());
}

static int old_fpga_done = -1;

void fpga_task_run(void)
{
    Dev_fpga *d = get_dev_fpga();
    if (fpga_done_pin_present()) {
        d->initb = HAL_GPIO_ReadPin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin);
        d->done = HAL_GPIO_ReadPin(FPGA_DONE_GPIO_Port, FPGA_DONE_Pin);
    } else {
        d->initb = 1;
        d->done = 1;
    }
   int fpga_core_power_present = get_fpga_core_power_present(get_dev_powermon_const()->sensors);
    int fpga_power_present = enable_power && fpga_core_power_present;
    int fpga_enable = fpga_power_present && d->initb;
//    int fpga_loading = fpga_power_present && d->initb && !d->done;
    int fpga_done = fpga_power_present && d->done;
    if (old_fpga_done != fpga_done) {
        old_fpga_done = fpga_done;
        if (fpga_done)
            fpga_enable_interface();
        else
            fpga_disable_interface();
    }
    switch (state) {
    case FPGA_STATE_STANDBY:
        if (fpga_enable) {
            state = FPGA_STATE_LOAD;
            fpga_load_start_tick = osKernelSysTick();
        }
        struct_fpga_init(d);
        break;
    case FPGA_STATE_LOAD:
        if (!fpga_enable) {
            state = FPGA_STATE_STANDBY;
            break;
        }
        if (fpga_done) {
            if (fpga_done_pin_present()) {
                const uint32_t tick_freq_hz = 1000U / HAL_GetTickFreq();
                const uint32_t ticks = osKernelSysTick() - fpga_load_start_tick;
                log_printf(LOG_INFO, "FPGA loaded in %u ms", ticks * 1000 / tick_freq_hz);
            }
            state = FPGA_STATE_RESET;
            break;
        }
        if (stateTicks() > LOAD_DELAY_TICKS) {
            log_put(LOG_ERR, "FPGA load timeout");
            state = FPGA_STATE_ERROR;
        }
        break;
    case FPGA_STATE_RESET:
        if (!fpga_power_present) {
            state = FPGA_STATE_STANDBY;
            break;
        }
        if (1
                && DEVICE_NORMAL == fpgaDetect(d)
                && DEVICE_NORMAL == fpga_test()
                ) {
            state = FPGA_STATE_RUN;
            if (! fpga_done_pin_present()) {
                const uint32_t tick_freq_hz = 1000U / HAL_GetTickFreq();
                const uint32_t ticks = osKernelSysTick() - fpga_load_start_tick;
                log_printf(LOG_INFO, "FPGA loaded in %u ms", ticks * 1000 / tick_freq_hz);
            }
        }
        {
            uint32_t detect_timeout = DETECT_DELAY_TICKS;
            if (! fpga_done_pin_present())
                detect_timeout += LOAD_DELAY_TICKS;
            if (stateTicks() > detect_timeout) {
                log_put(LOG_ERR, "FPGA detect timeout");
                state = FPGA_STATE_ERROR;
            }
        }
        break;
    case FPGA_STATE_RUN:
        if (!fpga_power_present) {
            state = FPGA_STATE_STANDBY;
            break;
        }
        if (!fpga_done)
            state = FPGA_STATE_STANDBY;
        if ((!fpga_check_live_magic()) ||
            (!fpgaWriteBmcVersion()) ||
            (!fpgaWriteBmcTemperature(get_dev_thset())) ||
            (!fpgaWritePllStatus(get_dev_pll())) ||
            (!fpgaWriteSystemStatus(getDevicesConst())) ||
            (!fpgaWriteSensors(get_dev_powermon_const()))
            ) {
            log_printf(LOG_ERR, "FPGA SPI error");
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
    old_state = state;
}
