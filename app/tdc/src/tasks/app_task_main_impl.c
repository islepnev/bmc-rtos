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

static uint32_t mainloopCount = 0;

void task_main_init(void)
{

}

void task_main_run(void)
{
    mainloopCount++;
    const PmState pmState = get_dev_powermon_const()->pmState;
    enable_pll_run = (pmState == PM_STATE_RUN);

    const SensorStatus systemStatus = getSystemStatus();
    led_set_state(LED_RED, systemStatus >= SENSOR_CRITICAL);
    led_set_state(LED_YELLOW, systemStatus >= SENSOR_WARNING);
    led_set_state(LED_GREEN, systemStatus == SENSOR_NORMAL);
}
