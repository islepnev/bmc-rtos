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

#include "app_task_sfpiic_impl.h"
#include "cmsis_os.h"
#include "dev_sfpiic.h"
#include "dev_sfpiic_types.h"
#include "app_shared_data.h"
#include "logbuffer.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

typedef enum {
    SFPIIC_STATE_RESET,
    SFPIIC_STATE_RUN,
    SFPIIC_STATE_PAUSE,
    SFPIIC_STATE_ERROR,
} sfpiic_state_t;

static sfpiic_state_t state = SFPIIC_STATE_RESET;
static sfpiic_state_t old_state = SFPIIC_STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static void struct_sfpiic_init(Dev_sfpiic *d)
{
    d->dev.device_status = DEVICE_UNKNOWN;
}

void task_sfpiic_run(void)
{
    Dev_sfpiic *d = get_dev_sfpiic();
    switch (state) {
    case SFPIIC_STATE_RESET: {
        struct_sfpiic_init(d);
        if (DEVICE_NORMAL == dev_sfpiic_detect(d))
            state = SFPIIC_STATE_RUN;
        break;
    }
    case SFPIIC_STATE_RUN:
        if (DEVICE_NORMAL != dev_sfpiic_update(d)) {
            state = SFPIIC_STATE_ERROR;
            break;
        }
        state = SFPIIC_STATE_PAUSE;
        break;
    case SFPIIC_STATE_PAUSE:
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = SFPIIC_STATE_RUN;
        }
        break;
    case SFPIIC_STATE_ERROR:
        if (old_state != state) {
            log_printf(LOG_ERR, "SFP IIC error");
        }
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = SFPIIC_STATE_RESET;
        }
        break;
    }
    if (old_state != state) {
        old_state = state;
        stateStartTick = osKernelSysTick();
    }
}
