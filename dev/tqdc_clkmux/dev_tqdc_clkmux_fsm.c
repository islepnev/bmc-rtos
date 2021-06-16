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
#include "dev_tqdc_clkmux.h"
#include "dev_tqdc_clkmux_types.h"
#include "device_status_log.h"
#include "log/log.h"

static const uint32_t MAX_ERROR_COUNT = 3;
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;

static uint32_t error_count = 0;

void dev_tqdc_clkmux_run(Dev_tqdc_clkmux *d)
{
    const DeviceStatus old_device_status = d->dev.device_status;
    dev_fsm_t *fsm = &d->dev.fsm;

    bool power_on = enable_power && system_power_present;
    if (!power_on) {
        dev_fsm_change(fsm, DEV_FSM_SHUTDOWN);
    }
    switch (fsm->state) {
    case DEV_FSM_SHUTDOWN: {
        if (power_on)
            dev_fsm_change(fsm, DEV_FSM_RESET);
        set_device_status(&d->dev, DEVICE_UNKNOWN);
        break;
    }
    case DEV_FSM_RESET: {
        dev_tqdc_clkmux_init(d);
        if (dev_tqdc_clkmux_detect(d)) {
            dev_fsm_change(fsm, DEV_FSM_RUN);
            break;
        }
        log_printf(LOG_WARNING, "IIC %d.%2X %s detect failed",
                   d->dev.bus.bus_number, d->dev.bus.address, d->dev.name);
        error_count++;
        if (error_count >= MAX_ERROR_COUNT)
            dev_fsm_change(fsm, DEV_FSM_ERROR);
        break;
    }
    case DEV_FSM_RUN:
        if (! dev_tqdc_clkmux_set(d)) {
            dev_fsm_change(fsm, DEV_FSM_ERROR);
        }
        set_device_status(&d->dev, DEVICE_NORMAL);
        dev_fsm_change(fsm, DEV_FSM_PAUSE);
        break;
    case DEV_FSM_PAUSE:
        if (dev_fsm_stateTicks(fsm) > POLL_DELAY_TICKS) {
            dev_fsm_change(fsm, DEV_FSM_RUN);
        }
        break;
    case DEV_FSM_ERROR:
        set_device_status(&d->dev, DEVICE_FAIL);
        if (dev_fsm_stateTicks(fsm) > ERROR_DELAY_TICKS) {
            error_count = 0;
            dev_fsm_change(fsm, DEV_FSM_RESET);
        }
        break;
    }
    if (old_device_status != d->dev.device_status)
        dev_log_status_change(&d->dev);
}
