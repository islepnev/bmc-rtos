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

#include "dev_max31725_fsm.h"

#include "cmsis_os.h"
#include "dev_max31725.h"
#include "app_shared_data.h"
#include "log.h"
#include "device_status_log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

static uint32_t stateTicks(Dev_max31725 *d)
{
    return osKernelSysTick() - d->state_start_tick;
}

void dev_max31725_run(Dev_max31725 *d)
{
    dev_max31725_state_t old_state = d->state;
    switch (d->state) {
    case MAX31725_STATE_RESET: {
        if (dev_max31725_detect(d)) {
            d->state = MAX31725_STATE_RUN;
            d->device_status = DEVICE_NORMAL;
            dev_log_status_change(&d->bus, d->device_status);
            break;
        } else {
            d->state = MAX31725_STATE_ERROR;
        }
        if (stateTicks(d) > 2000) {
            d->device_status = DEVICE_UNKNOWN;
            dev_log_status_change(&d->bus, d->device_status);
            d->state = MAX31725_STATE_ERROR;
            break;
        }
        break;
    }
    case MAX31725_STATE_RUN:
        if (! dev_max31725_read(d)) {
            d->device_status = DEVICE_FAIL;
            dev_log_status_change(&d->bus, d->device_status);
            d->state = MAX31725_STATE_ERROR;
            break;
        }
        log_printf(LOG_INFO, "MAX31725 temperature %f", d->temp);
        d->state = MAX31725_STATE_PAUSE;
        break;
    case MAX31725_STATE_PAUSE:
        if (stateTicks(d) > POLL_DELAY_TICKS) {
            d->state = MAX31725_STATE_RUN;
        }
        break;
    case MAX31725_STATE_ERROR:
        if (stateTicks(d) > ERROR_DELAY_TICKS) {
            d->state = MAX31725_STATE_RESET;
        }
        break;
    default:
        d->state = MAX31725_STATE_RESET;
    }

    if (old_state != d->state) {
        d->state_start_tick = osKernelSysTick();
    }
}
