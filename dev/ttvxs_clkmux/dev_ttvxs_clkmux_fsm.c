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

#include "dev_ttvxs_clkmux_fsm.h"

#include "app_shared_data.h"
#include "cmsis_os.h"
#include "device_status_log.h"
#include "log/log.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux_types.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 2000;
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;

static uint32_t stateTicks(const Dev_ttvxs_clkmux_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

static void struct_vxs_i2c_init(Dev_ttvxs_clkmux *d)
{
    d->dev.device_status = DEVICE_UNKNOWN;
    d->priv.pll_source = TTVXS_PLL_SOURCE_DIV3;
}

static void change_fsm_state(Dev_ttvxs_clkmux_priv *p, const ttvxs_clkmux_state_t state)
{
    if (state == p->fsm_state)
        return;
    p->stateStartTick = osKernelSysTick();
    p->fsm_state = state;
}

void dev_ttvxs_clkmux_run(Dev_ttvxs_clkmux *d)
{
    Dev_ttvxs_clkmux_priv *p = &d->priv;
    const DeviceStatus old_device_status = d->dev.device_status;
    const bool power_on = enable_power && system_power_present;
    if (!power_on) {
        change_fsm_state(p, TTVXS_CLKMUX_STATE_SHUTDOWN);
        set_device_status(&d->dev, DEVICE_UNKNOWN);
    }
    switch (p->fsm_state) {
    case TTVXS_CLKMUX_STATE_SHUTDOWN: {
        if (power_on)
            change_fsm_state(p, TTVXS_CLKMUX_STATE_RESET);
        break;
    }
    case TTVXS_CLKMUX_STATE_RESET: {
        struct_vxs_i2c_init(d);
        DeviceStatus status = dev_ttvxs_clkmux_detect(d);
        if (status == DEVICE_NORMAL) {
            change_fsm_state(p, TTVXS_CLKMUX_STATE_RUN);
            break;
        }
        if (stateTicks(p) > DETECT_TIMEOUT_TICKS) {
            set_device_status(&d->dev, DEVICE_UNKNOWN);
            change_fsm_state(p, TTVXS_CLKMUX_STATE_ERROR);
            break;
        }
        break;
    }
    case TTVXS_CLKMUX_STATE_RUN:
        if (DEVICE_NORMAL == dev_ttvxs_clkmux_set(d)) {
            change_fsm_state(p, TTVXS_CLKMUX_STATE_PAUSE);
        } else {
            change_fsm_state(p, TTVXS_CLKMUX_STATE_ERROR);
            log_printf(LOG_ERR, "CLKMUX IIC error");
        }
        break;
    case TTVXS_CLKMUX_STATE_PAUSE:
        if (stateTicks(p) > POLL_DELAY_TICKS) {
            change_fsm_state(p, TTVXS_CLKMUX_STATE_RUN);
        }
        break;
    case TTVXS_CLKMUX_STATE_ERROR:
        if (stateTicks(p) > ERROR_DELAY_TICKS) {
            change_fsm_state(p, TTVXS_CLKMUX_STATE_RESET);
        }
        break;
    }

    if (old_device_status != d->dev.device_status)
        dev_log_status_change(&d->dev);
}
