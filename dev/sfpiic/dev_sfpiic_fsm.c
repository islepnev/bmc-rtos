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

static const uint32_t POWERON_DELAY_TICKS = 2000; // 450 ms minimum
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

void task_sfpiic_run(Dev_sfpiic *p, bool power_on)
{
    const sfpiic_state_t old_state = p->priv.fsm_state;
    const DeviceStatus old_device_status = p->dev.device_status;
    const uint32_t stateTicks = osKernelSysTick() - p->priv.stateStartTick;

    if (!power_on) {
        if (p->priv.fsm_state != SFPIIC_STATE_SHUTDOWN) {
            p->priv.fsm_state = SFPIIC_STATE_SHUTDOWN;
            static const sfpiic_status_t zz = {0};
            p->priv.status = zz;
            p->dev.device_status = DEVICE_UNKNOWN;
        }
    }
    switch (p->priv.fsm_state) {
    case SFPIIC_STATE_SHUTDOWN: {
        p->dev.device_status = DEVICE_UNKNOWN;
        if (power_on)
            p->priv.fsm_state = SFPIIC_STATE_RESET;
        break;
    }
    case SFPIIC_STATE_RESET: {
        if (stateTicks > POWERON_DELAY_TICKS) {
            if (DEVICE_NORMAL == dev_sfpiic_detect(p)) {
                p->priv.fsm_state = SFPIIC_STATE_RUN;
            }
        }
        break;
    }
    case SFPIIC_STATE_RUN:
        if (DEVICE_NORMAL != dev_sfpiic_update(p)) {
            p->priv.fsm_state = SFPIIC_STATE_ERROR;
            break;
        }
        p->priv.fsm_state = SFPIIC_STATE_PAUSE;
        break;
    case SFPIIC_STATE_PAUSE:
        if (stateTicks > POLL_DELAY_TICKS) {
            p->priv.fsm_state = SFPIIC_STATE_RUN;
        }
        break;
    case SFPIIC_STATE_ERROR:
        if (old_state != p->priv.fsm_state) {
        }
        if (stateTicks > ERROR_DELAY_TICKS) {
            p->priv.fsm_state = SFPIIC_STATE_RESET;
        }
        break;
    }
    if (old_device_status != p->dev.device_status)
        dev_log_status_change(&p->dev);

    if (old_state != p->priv.fsm_state) {
        p->priv.stateStartTick = osKernelSysTick();
    }
}
