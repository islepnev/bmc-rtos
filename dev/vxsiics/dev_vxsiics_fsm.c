/*
**    Copyright 2019-2021 Ilja Slepnev
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

#include "dev_vxsiics_fsm.h"

#include "cmsis_os.h"
#include "dev_vxsiics.h"
#include "dev_vxsiics_types.h"
#include "log/log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;


static vxsiics_state_t state = VXSIICS_STATE_RESET;
static vxsiics_state_t old_state = VXSIICS_STATE_RESET;

static uint32_t stateTicks(const Dev_vxsiics_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

void dev_vxsiics_run(Dev_vxsiics *d)
{
    switch (state) {
    case VXSIICS_STATE_RESET: {
        d->dev.device_status = DEVICE_NORMAL;
        state = VXSIICS_STATE_RUN;
        break;
    }
    case VXSIICS_STATE_RUN:
        dev_vxsiics_poll_status(d);
        state = VXSIICS_STATE_PAUSE;
        break;
    case VXSIICS_STATE_PAUSE:
        if (stateTicks(&d->priv) > POLL_DELAY_TICKS) {
            state = VXSIICS_STATE_RUN;
        }
        break;
    case VXSIICS_STATE_ERROR:
        d->dev.device_status = DEVICE_FAIL;
        if (old_state != state) {
            log_printf(LOG_ERR, "VXS IIC error");
        }
        if (stateTicks(&d->priv) > ERROR_DELAY_TICKS) {
            state = VXSIICS_STATE_RESET;
        }
        break;
    }
    d->dev.sensor = SENSOR_NORMAL;

    if (old_state != state) {
        old_state = state;
        d->priv.stateStartTick = osKernelSysTick();
    }
}
