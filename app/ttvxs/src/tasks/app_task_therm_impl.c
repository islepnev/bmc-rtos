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

#include "app_task_therm_impl.h"
#include "cmsis_os.h"
#include "max31725/dev_max31725.h"
#include "app_shared_data.h"
#include "log.h"
#include "device_status_log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

typedef enum {
    STATE_RESET,
    STATE_RUN,
    STATE_PAUSE,
    STATE_ERROR,
} state_t;

static state_t state = STATE_RESET;
static state_t old_state = STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void task_therm_run(void)
{
    Dev_max31725 *d = get_dev_max31725();
    switch (state) {
    case STATE_RESET: {
        if (dev_max31725_detect(d) && dev_max31725_read(d)) {
            state = STATE_RUN;
            d->present = DEVICE_NORMAL;
            dev_log_status_change(&d->bus, d->present);
            break;
        } else {
            state = STATE_ERROR;
        }
        if (stateTicks() > 2000) {
            d->present = DEVICE_UNKNOWN;
            dev_log_status_change(&d->bus, d->present);
            state = STATE_ERROR;
            break;
        }
        break;
    }
    case STATE_RUN:
        if (! dev_max31725_read(d)) {
            d->present = DEVICE_FAIL;
            dev_log_status_change(&d->bus, d->present);
            state = STATE_ERROR;
            break;
        }
        log_printf(LOG_INFO, "MAX31725 temperature %f", d->temp);
        state = STATE_PAUSE;
        break;
    case STATE_PAUSE:
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = STATE_RUN;
        }
        break;
    case STATE_ERROR:
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = STATE_RESET;
        }
        break;
    }
    if (old_state != state) {
        old_state = state;
        stateStartTick = osKernelSysTick();
    }
}
