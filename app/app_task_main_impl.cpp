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

#include "app_task_main_impl.h"

#include "dev_powermon_types.h"
#include "debug_helpers.h"
#include "logbuffer.h"
#include "system_status.h"

#include "app_shared_data.h"
#include "led_gpio_hal.h"
#include "bsp_pin_defs.h"
#include "devices_types.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

static const uint32_t MAIN_DETECT_TIMEOUT_TICKS = 5000;

static uint32_t mainloopCount = 0;

typedef enum {
    MAIN_STATE_INIT,
    MAIN_STATE_DETECT,
    MAIN_STATE_RUN,
    MAIN_STATE_ERROR
} MainState;

//MainState getMainState(void);
static MainState state = MAIN_STATE_INIT;

//MainState getMainState(void)
//{
//    return mainState;
//}

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void task_main_init(void)
{

}

void task_main_run(void)
{
    mainloopCount++;
    const MainState old_state = state;
    const PmState pmState = get_dev_powermon_const()->pmState;
//    led_toggle(LED_YELLOW);

    switch (state) {
    case MAIN_STATE_INIT:
        if (pmState == PM_STATE_RUN) {
            state = MAIN_STATE_DETECT;
        }
        break;
    case MAIN_STATE_DETECT:
        if (pmState != PM_STATE_RUN) {
            state = MAIN_STATE_INIT;
        }
        if (getDeviceStatus(getDevicesConst()) == DEVICE_NORMAL)
            state = MAIN_STATE_RUN;
        if (stateTicks() > MAIN_DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "DETECT timeout");
            state = MAIN_STATE_ERROR;
        }
        break;
    case MAIN_STATE_RUN:
        if (getDeviceStatus(getDevicesConst()) != DEVICE_NORMAL)
            state = MAIN_STATE_ERROR;
        if (pmState != PM_STATE_RUN) {
            state = MAIN_STATE_INIT;
        }
        break;
    case MAIN_STATE_ERROR:
        if (old_state != state) {
            log_printf(LOG_ERR, "System error");
        }
        if (stateTicks() > 2000) {
            state = MAIN_STATE_INIT;
        }
        break;
    }

    enable_pll_run = (pmState == PM_STATE_RUN);
    if (old_state != state) {
        stateStartTick = osKernelSysTick();
    }

    const SensorStatus systemStatus = getSystemStatus(getDevicesConst());
    led_set_state(LED_RED, systemStatus >= SENSOR_CRITICAL);
    led_set_state(LED_YELLOW, systemStatus >= SENSOR_WARNING);
    led_set_state(LED_GREEN, systemStatus == SENSOR_NORMAL);
}
