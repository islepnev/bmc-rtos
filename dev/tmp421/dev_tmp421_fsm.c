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
#include "app_shared_data.h"
#include "log.h"
#include "device_status_log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

static uint32_t stateTicks(Dev_tmp421_priv *p)
{
    return osKernelSysTick() - p->state_start_tick;
}

void dev_tmp421_run(Dev_tmp421 *p)
{
    Dev_tmp421_priv *d = (Dev_tmp421_priv *)&p->priv;
#ifdef BOARD_TTVXS
    if (!enable_power || !system_power_present) {
        if (d->state != TMP421_STATE_SHUTDOWN) {
            d->state = TMP421_STATE_SHUTDOWN;
            p->dev.device_status = DEVICE_UNKNOWN;
            log_put(LOG_INFO, "TMP421 shutdown");
        }
        return;
    }
#endif
    dev_tmp421_state_t old_state = d->state;
    switch (d->state) {
    case TMP421_STATE_SHUTDOWN: {
        d->state = TMP421_STATE_RESET;
        break;
    }
    case TMP421_STATE_RESET: {
        if (dev_tmp421_detect(p)) {
            d->state = TMP421_STATE_RUN;
            p->dev.device_status = DEVICE_NORMAL;
            dev_log_status_change(&p->dev.bus, p->dev.device_status);
            break;
        } else {
            d->state = TMP421_STATE_ERROR;
        }
        if (stateTicks(d) > 2000) {
            p->dev.device_status = DEVICE_UNKNOWN;
            dev_log_status_change(&p->dev.bus, p->dev.device_status);
            d->state = TMP421_STATE_ERROR;
            break;
        }
        break;
    }
    case TMP421_STATE_RUN:
        if (! dev_tmp421_read(p)) {
            p->dev.device_status = DEVICE_FAIL;
            dev_log_status_change(&p->dev.bus, p->dev.device_status);
            d->state = TMP421_STATE_ERROR;
            break;
        }
        // log_printf(LOG_INFO, "TMP421 temperature %.2f", d->temp);
        d->state = TMP421_STATE_PAUSE;
        break;
    case TMP421_STATE_PAUSE:
        if (stateTicks(d) > POLL_DELAY_TICKS) {
            d->state = TMP421_STATE_RUN;
        }
        break;
    case TMP421_STATE_ERROR:
        if (stateTicks(d) > ERROR_DELAY_TICKS) {
            d->state = TMP421_STATE_RESET;
        }
        break;
    default:
        d->state = TMP421_STATE_RESET;
    }

    if (old_state != d->state) {
        d->state_start_tick = osKernelSysTick();
    }
}
