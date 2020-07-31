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

#include "dev_vxsiicm_fsm.h"

#include "app_shared_data.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "dev_vxsiicm.h"
#include "dev_vxsiicm_types.h"
#include "devices.h"
#include "logbuffer.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 100;


static vxsiic_state_t state = VXSIIC_STATE_RESET;
static vxsiic_state_t old_state = VXSIIC_STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void dev_vxsiicm_run(Dev_vxsiicm *d)
{
    switch (state) {
    case VXSIIC_STATE_RESET: {
        struct_vxs_i2c_init(d);
        DeviceStatus status = dev_vxsiicm_detect(d);
        if (status == DEVICE_NORMAL)
            state = VXSIIC_STATE_RUN;
        break;
    }
    case VXSIIC_STATE_RUN:
        if (DEVICE_NORMAL == dev_vxsiicm_read(d))
            state = VXSIIC_STATE_PAUSE;
        else
            state = VXSIIC_STATE_ERROR;
        break;
    case VXSIIC_STATE_PAUSE:
        if (stateTicks() > POLL_DELAY_TICKS) {
            state = VXSIIC_STATE_RUN;
        }
        break;
    case VXSIIC_STATE_ERROR:
        if (old_state != state) {
            log_printf(LOG_ERR, "VXS IIC error");
        }
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = VXSIIC_STATE_RESET;
        }
        break;
    }
    if (old_state != state) {
        old_state = state;
        stateStartTick = osKernelSysTick();
    }
}
