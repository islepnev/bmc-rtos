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

#include "dev_adt7301_fsm.h"

#include "cmsis_os.h"
#include "dev_adt7301.h"
#include "device_status_log.h"
#include "log/log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

static uint32_t stateTicks(Dev_adt7301_priv *p)
{
    return osKernelSysTick() - p->state_start_tick;
}

void dev_adt7301_run(Dev_adt7301 *p, bool enable)
{
    Dev_adt7301_priv *d = (Dev_adt7301_priv *)&p->priv;
    if (!enable) {
        if (d->state != ADT7301_STATE_SHUTDOWN) {
            d->state = ADT7301_STATE_SHUTDOWN;
            p->dev.device_status = DEVICE_UNKNOWN;
            log_put(LOG_INFO, "ADT7301 shutdown");
        }
        return;
    }
    dev_adt7301_state_t old_state = d->state;
    switch (d->state) {
    case ADT7301_STATE_SHUTDOWN: {
        d->state = ADT7301_STATE_RESET;
        break;
    }
    case ADT7301_STATE_RESET: {
        if (dev_adt7301_detect(p)) {
            d->state = ADT7301_STATE_RUN;
            p->dev.device_status = DEVICE_NORMAL;
            dev_log_status_change(&p->dev);
            break;
        } else {
            d->state = ADT7301_STATE_ERROR;
        }
        if (stateTicks(d) > 2000) {
            p->dev.device_status = DEVICE_UNKNOWN;
            dev_log_status_change(&p->dev);
            d->state = ADT7301_STATE_ERROR;
            break;
        }
        break;
    }
    case ADT7301_STATE_RUN:
        if (! dev_adt7301_read(p)) {
            p->dev.device_status = DEVICE_FAIL;
            dev_log_status_change(&p->dev);
            d->state = ADT7301_STATE_ERROR;
            break;
        }
        // log_printf(LOG_INFO, "ADT7301 temperature %.2f", d->temp);
        d->state = ADT7301_STATE_PAUSE;
        break;
    case ADT7301_STATE_PAUSE:
        if (stateTicks(d) > POLL_DELAY_TICKS) {
            d->state = ADT7301_STATE_RUN;
        }
        break;
    case ADT7301_STATE_ERROR:
        if (stateTicks(d) > ERROR_DELAY_TICKS) {
            d->state = ADT7301_STATE_RESET;
        }
        break;
    default:
        d->state = ADT7301_STATE_RESET;
    }

    if (old_state != d->state) {
        d->state_start_tick = osKernelSysTick();
    }
}
