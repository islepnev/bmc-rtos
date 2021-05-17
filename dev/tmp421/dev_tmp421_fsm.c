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

#include "dev_tmp421_fsm.h"

#include "cmsis_os.h"
#include "dev_tmp421.h"
#include "device_status_log.h"
#include "log/log.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 2000;
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

void dev_tmp421_run(Dev_tmp421 *d)
{
    const DeviceStatus old_device_status = d->dev.device_status;
    dev_fsm_t *fsm = &d->dev.fsm;
    const bool power_on = true;
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
        if (dev_tmp421_detect(d)) {
            dev_fsm_change(fsm, DEV_FSM_RUN);
            break;
        }
        if (dev_fsm_stateTicks(fsm) > DETECT_TIMEOUT_TICKS) {
            dev_fsm_change(fsm, DEV_FSM_ERROR);
            break;
        }
        break;
    }
    case DEV_FSM_RUN:
        if (! dev_tmp421_read(d)) {
            dev_fsm_change(fsm, DEV_FSM_ERROR);
            break;
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
            dev_fsm_change(fsm, DEV_FSM_RESET);
        }
        break;
    default:
        dev_fsm_change(fsm, DEV_FSM_RESET);
    }
    if (old_device_status != d->dev.device_status)
        dev_log_status_change(&d->dev);
}
