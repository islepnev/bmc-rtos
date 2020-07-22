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

#include "app_task_clkmux_impl.h"
#include "cmsis_os.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux_types.h"
#include "app_shared_data.h"
#include "debug_helpers.h"
#include "logbuffer.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;

typedef enum {
    CLKMUX_STATE_RESET,
    CLKMUX_STATE_RUN,
    CLKMUX_STATE_PAUSE,
    CLKMUX_STATE_ERROR,
} clkmux_state_t;

static clkmux_state_t state = CLKMUX_STATE_RESET;
static clkmux_state_t old_state = CLKMUX_STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static void struct_vxs_i2c_init(Dev_ttvxs_clkmux *d)
{
    d->present = DEVICE_UNKNOWN;
    d->pll_source = TTVXS_PLL_SOURCE_DIV3;
}

void task_clkmux_init()
{
    Dev_ttvxs_clkmux *d = get_dev_clkmux();
    dev_ttvxs_clkmux_init(d);
}

void task_clkmux_run(void)
{
    Dev_ttvxs_clkmux *d = get_dev_clkmux();
    if (!enable_power || !system_power_present) {
        state = CLKMUX_STATE_RESET;
        d->present = DEVICE_UNKNOWN;
        return;
    }
    switch (state) {
    case CLKMUX_STATE_RESET: {
        struct_vxs_i2c_init(d);
        DeviceStatus status = dev_ttvxs_clkmux_detect(d);
        if (status == DEVICE_NORMAL)
            state = CLKMUX_STATE_RUN;
        break;
    }
    case CLKMUX_STATE_RUN:
        if (DEVICE_NORMAL == dev_ttvxs_clkmux_set(d))
            state = CLKMUX_STATE_PAUSE;
        else
            state = CLKMUX_STATE_ERROR;
        break;
    case CLKMUX_STATE_PAUSE:
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = CLKMUX_STATE_RUN;
        }
        break;
    case CLKMUX_STATE_ERROR:
        if (old_state != state) {
            log_printf(LOG_ERR, "CLKMUX IIC error");
        }
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = CLKMUX_STATE_RESET;
        }
        break;
    }
    if (old_state != state) {
        old_state = state;
        stateStartTick = osKernelSysTick();
    }
}
