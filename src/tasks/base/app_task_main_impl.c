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
#include "bsp.h"
#include "device_status_log.h"
#include "led_gpio_hal.h"
#include "log/log.h"
//#include "powermon/dev_powermon_types.h"
#include "system_status.h"

static uint32_t mainloopCount = 0;

void check_clock_ready(void)
{
    static bool old_clock_ready = false;
    bool clock_ready = enable_power && system_power_present;
#if defined(ENABLE_AD9545) || defined(ENABLE_AD9548)
    clock_ready &= main_clock_ready;
#endif

#if defined(ENABLE_AD9516)
    clock_ready &= aux_clock_ready;
#endif

    if (old_clock_ready != clock_ready) {
        bsp_update_system_powergood_pin(clock_ready);
        if (clock_ready)
            log_put(LOG_INFO, "clock ready");
        else
            log_put(LOG_WARNING, "clock not ready");
        old_clock_ready = clock_ready;
    }
}

void task_main_init(void)
{

}

SensorStatus old_status = SENSOR_UNKNOWN;

void task_main_run(void)
{
    mainloopCount++;
//    const PmState pmState = get_powermon_state();
    // enable_pll_run = (pmState == PM_STATE_RUN);

    const SensorStatus systemStatus = getSystemStatus();
    if (old_status != systemStatus) {
        LogPriority prio = sensor_status_log_priority(systemStatus);
        log_printf(prio, "System status is %s", sensor_status_text(systemStatus));
        old_status = systemStatus;
    }
    check_clock_ready();

    // read SD card status
    // getDevices()->sd.detect_b = read_gpio_pin(uSD_Detect_GPIO_Port, uSD_Detect_Pin);
    // read other signals
    // getDevices()->pen_b = read_gpio_pin(PEN_B_GPIO_Port, PEN_B_Pin);

    static int cnt = 0;
    cnt++;
    if (cnt > 50)
        cnt = 0;
    bool blink = cnt < 2;
    led_set_state(LED_RED,    !blink & (systemStatus >= SENSOR_CRITICAL));
    led_set_state(LED_YELLOW, !blink & (systemStatus >= SENSOR_WARNING));
    led_set_state(LED_GREEN,  !blink & (systemStatus == SENSOR_NORMAL));
    led_set_state(LED_INT_GREEN, blink);
}
