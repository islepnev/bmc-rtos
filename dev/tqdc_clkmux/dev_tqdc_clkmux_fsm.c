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

#include "dev_tqdc_clkmux_fsm.h"

#include "app_shared_data.h"
#include "cmsis_os.h"
#include "log/log.h"
#include "tqdc_clkmux/dev_tqdc_clkmux.h"
#include "tqdc_clkmux/dev_tqdc_clkmux_types.h"

static const uint32_t MAX_ERROR_COUNT = 3;
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;

static uint32_t stateTicks(const Dev_tqdc_clkmux_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

static uint32_t error_count = 0;

void dev_tqdc_clkmux_run(Dev_tqdc_clkmux *d)
{
    Dev_tqdc_clkmux_priv *p = &d->priv;
    tqdc_clkmux_fsm_state_t old_state = p->fsm_state;
    if (!enable_power || !system_power_present) {
        p->fsm_state = TQDC_CLKMUX_STATE_RESET;
        d->dev.device_status = DEVICE_UNKNOWN;
        return;
    }
    switch (p->fsm_state) {
    case TQDC_CLKMUX_STATE_RESET: {
        dev_tqdc_clkmux_init(d);
        DeviceStatus status = dev_tqdc_clkmux_detect(d);
        if (status == DEVICE_NORMAL)
            p->fsm_state = TQDC_CLKMUX_STATE_RUN;
        log_printf(LOG_WARNING, "IIC %d.%2X %s detect failed",
                   d->dev.bus.bus_number, d->dev.bus.address, d->dev.name);
        error_count++;
        if (error_count >= MAX_ERROR_COUNT)
            p->fsm_state = TQDC_CLKMUX_STATE_ERROR;
        break;
    }
    case TQDC_CLKMUX_STATE_RUN:
        if (DEVICE_NORMAL == dev_tqdc_clkmux_set(d)) {
            p->fsm_state = TQDC_CLKMUX_STATE_PAUSE;
        } else {
            p->fsm_state = TQDC_CLKMUX_STATE_ERROR;
            log_printf(LOG_ERR, "CLKMUX IIC error");
        }
        break;
    case TQDC_CLKMUX_STATE_PAUSE:
        if (stateTicks(p) > POLL_DELAY_TICKS) {
            p->fsm_state = TQDC_CLKMUX_STATE_RUN;
        }
        break;
    case TQDC_CLKMUX_STATE_ERROR:
        if (stateTicks(p) > ERROR_DELAY_TICKS) {
            error_count = 0;
            p->fsm_state = TQDC_CLKMUX_STATE_RESET;
        }
        break;
    }
    if (old_state != p->fsm_state) {
        p->stateStartTick = osKernelSysTick();
    }
}
