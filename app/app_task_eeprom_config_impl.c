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

#include "app_task_eeprom_config_impl.h"
#include "cmsis_os.h"
#include "dev_eeprom.h"
#include "dev_eeprom_types.h"
#include "app_shared_data.h"

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

static void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

static DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    if (HAL_OK == dev_eepromConfig_Detect())
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromConfig_Read(0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

void task_eeprom_config_run(void)
{
    Dev_at24c *d = get_dev_eeprom_config();
    switch (state) {
    case STATE_RESET: {
        struct_at24c_init(d);
        DeviceStatus status = dev_eepromConfig_detect(d);
        if (status == DEVICE_NORMAL)
            state = STATE_RUN;
        break;
    }
    case STATE_RUN:
//        if (HAL_OK != dev_eepromConfig_Read()) {
//            state = STATE_ERROR;
//            break;
//        }
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
        stateStartTick = osKernelSysTick();
    }
}
