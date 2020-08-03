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

#include "dev_sfpiic_fsm.h"

#include "cmsis_os.h"
#include "dev_sfpiic.h"
#include "dev_sfpiic_types.h"
#include "device_status_log.h"
#include "log/log.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

static uint32_t stateTicks(const Dev_sfpiic_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

void task_sfpiic_run(Dev_sfpiic *p, bool power_on)
{
    sfpiic_state_t old_state = p->priv.state;
    if (!power_on) {
        if (p->priv.state != SFPIIC_STATE_SHUTDOWN) {
            p->priv.state = SFPIIC_STATE_SHUTDOWN;
            p->dev.device_status = DEVICE_UNKNOWN;
            log_put(LOG_INFO, "SFP IIC shutdown");
        }
    }
    switch (p->priv.state) {
    case SFPIIC_STATE_SHUTDOWN: {
        p->dev.device_status = DEVICE_UNKNOWN;
        if (power_on)
            p->priv.state = SFPIIC_STATE_RESET;
        break;
    }
    case SFPIIC_STATE_RESET: {
        if (DEVICE_NORMAL == dev_sfpiic_detect(p)) {
            p->priv.state = SFPIIC_STATE_RUN;
            dev_log_status_change(&p->dev);
        }
        break;
    }
    case SFPIIC_STATE_RUN:
        if (DEVICE_NORMAL != dev_sfpiic_update(p)) {
            p->priv.state = SFPIIC_STATE_ERROR;
            dev_log_status_change(&p->dev);
            break;
        }
        p->priv.state = SFPIIC_STATE_PAUSE;
        break;
    case SFPIIC_STATE_PAUSE:
        if (stateTicks(&p->priv) > POLL_DELAY_TICKS) {
            p->priv.state = SFPIIC_STATE_RUN;
        }
        break;
    case SFPIIC_STATE_ERROR:
        if (old_state != p->priv.state) {
            log_printf(LOG_ERR, "SFP IIC error");
        }
        if (stateTicks(&p->priv) > ERROR_DELAY_TICKS) {
            p->priv.state = SFPIIC_STATE_RESET;
        }
        break;
    }
    if (old_state != p->priv.state) {
        old_state = p->priv.state;
        p->priv.stateStartTick = osKernelSysTick();
    }
}
