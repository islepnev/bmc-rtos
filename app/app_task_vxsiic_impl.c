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

#include "app_task_vxsiic_impl.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal_gpio.h"
#include "dev_vxsiic.h"
//#include "dev_types.h"
#include "app_shared_data.h"
#include "debug_helpers.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

typedef enum {
    VXSIIC_STATE_RESET,
    VXSIIC_STATE_RUN,
    VXSIIC_STATE_PAUSE,
    VXSIIC_STATE_ERROR,
} vxsiic_state_t;

static vxsiic_state_t state = VXSIIC_STATE_RESET;
static vxsiic_state_t old_state = VXSIIC_STATE_RESET;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static void struct_vxs_i2c_init(Dev_vxsiic *d)
{
    d->present = DEVICE_UNKNOWN;
    for (int i=0; i<VXSIIC_SLOTS; i++) {
        vxsiic_slot_status_t zz = {0};
        d->status.slot[i] = zz;
    }
}

void task_vxsiic_init(void)
{

}

void task_vxsiic_run(void)
{
    Dev_vxsiic *d = get_dev_vxsiic();
    switch (state) {
    case VXSIIC_STATE_RESET: {
        struct_vxs_i2c_init(d);
        DeviceStatus status = dev_vxsiic_detect(d);
        if (status == DEVICE_NORMAL)
            state = VXSIIC_STATE_RUN;
        break;
    }
    case VXSIIC_STATE_RUN:
        if (HAL_OK == dev_vxsiic_read(d))
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
