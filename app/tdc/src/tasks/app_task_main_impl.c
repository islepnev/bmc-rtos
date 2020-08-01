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

#include "app_shared_data.h"
#include "device_status_log.h"
#include "led_gpio_hal.h"
#include "log/log.h"
#include "powermon/dev_powermon_types.h"
#include "system_status.h"

static uint32_t mainloopCount = 0;

void task_main_init(void)
{

}

SensorStatus old_status = SENSOR_UNKNOWN;

void task_main_run(void)
{
    mainloopCount++;
    const PmState pmState = get_powermon_state();
    enable_pll_run = (pmState == PM_STATE_RUN);

    const SensorStatus systemStatus = getSystemStatus();
    if (old_status != systemStatus) {
        LogPriority prio = sensor_status_log_priority(systemStatus);
        log_printf(prio, "System status is %s", sensor_status_text(systemStatus));
        old_status = systemStatus;
    }


    // read SD card status
    // getDevices()->sd.detect_b = HAL_GPIO_ReadPin(uSD_Detect_GPIO_Port, uSD_Detect_Pin);
    // read other signals
    // getDevices()->pen_b = HAL_GPIO_ReadPin(PEN_B_GPIO_Port, PEN_B_Pin);

    led_set_state(LED_RED, systemStatus >= SENSOR_CRITICAL);
    led_set_state(LED_YELLOW, systemStatus >= SENSOR_WARNING);
    led_set_state(LED_GREEN, systemStatus == SENSOR_NORMAL);
}
