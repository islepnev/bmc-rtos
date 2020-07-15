//
//    Copyright 2019-2020 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "app_task_powermon_impl.h"

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os.h"

#include "dev_pm_sensors_types.h"
#include "dev_pm_sensors_config.h"
#include "dev_pm_sensors.h"
#include "bsp_powermon.h"
#include "dev_powermon.h"
#include "dev_common_types.h"
#include "dev_powermon_types.h"
#include "dev_thset.h"
#include "logbuffer.h"
#include "app_shared_data.h"
#include "bsp.h"

static const int TEST_RESTART = 0; // debug only
static const uint32_t INIT_TIMEOUT_TICKS = 500;
static const uint32_t RAMP_TIMEOUT_TICKS = 3000;
static const uint32_t POWERFAIL_DELAY_TICKS = 3000;
static const uint32_t ERROR_DELAY_TICKS = 3000;

static const uint32_t log_sensor_status_duration_ticks = 3000;

uint32_t pmLoopCount = 0;

static const uint32_t sensor_read_interval_run = 100;
static const uint32_t sensor_read_interval_ramp = 10;
static uint32_t sensorReadTick = 0;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static SensorStatus oldSensorStatus[POWERMON_SENSORS] = {SENSOR_UNKNOWN};
static void clearOldSensorStatus(void)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        oldSensorStatus[i] = SENSOR_NORMAL;
}

static void log_sensor_status_change(const Dev_powermon *pm)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &pm->sensors[i];
        if (sensor->rampState != RAMP_NONE)
            continue;
        SensorStatus status = pm_sensor_status(sensor);
        if (status != oldSensorStatus[i]) {
            enum { size = 50 };
            static char str[size];
            const double volt = sensor->busVoltage;
            const int volt_int = volt;
            int volt_frac = 1000 * (volt - volt_int);
            if (volt_frac < 0) volt_frac = -volt_frac;
            double curr = sensor->current;
            int neg = (curr < 0);
            if (neg) curr = -curr;
            const int curr_int = curr;
            int curr_frac = 1000 * (curr - curr_int);
            snprintf(str, size, "%s %s, %d.%03d V, %s%d.%03d A",
                     sensor->label,
                     sensor_status_text(status),
                     volt_int,
                     volt_frac,
                     neg?"-":"",
                     curr_int,
                     curr_frac
                     );
            switch(status) {
            case SENSOR_UNKNOWN:
                log_put(LOG_WARNING, str);
                break;
            case SENSOR_CRITICAL:
                log_put(LOG_ERR, str);
                break;
            case SENSOR_WARNING:
                log_put(LOG_WARNING, str);
                break;
            case SENSOR_NORMAL:
                if (pm_sensor_get_sensorStatus_Duration(sensor) >= log_sensor_status_duration_ticks)
                    log_put(LOG_INFO, str);
                break;
            }
            oldSensorStatus[i] = status;
        }
    }
}

static int pm_initialized = 0;
static bool old_inut_power_normal = false;
static bool old_inut_power_critical = false;

void powermon_task_init(void)
{
    clearOldSensorStatus();
    dev_thset_init(get_dev_thset());
}

void change_state(PmState state)
{
    Dev_powermon *pm = get_dev_powermon();
    if (pm->pmState == state)
        return;
    pm->pmState = state;
    stateStartTick = osKernelSysTick();
}

void task_powermon_run (void)
{
    Dev_powermon *pm = get_dev_powermon();
    if (!pm_initialized) {
        powermon_task_init();
        pm_initialized = 1;
    }
    pmLoopCount++;
    bool vmePresent = pm_read_liveInsert(pm);
    pm_read_pgood(&pm->pgood);
    const bool input_power_normal = get_input_power_normal(pm->sensors);
    if (input_power_normal != old_inut_power_normal) {
        if (input_power_normal)
            log_put(LOG_NOTICE, "Input power normal");
        old_inut_power_normal = input_power_normal;
    }
    const int input_power_critical = get_input_power_failed(pm->sensors);
    if (input_power_critical != old_inut_power_critical) {
        if (input_power_critical)
            log_put(LOG_WARNING, "Input power critical");
        old_inut_power_critical = input_power_critical;
    }
    const int power_critical_ok = get_critical_power_valid(pm->sensors);
    const int power_critical_failure = get_critical_power_failure(pm->sensors);

//    const thset_state_t thset_state = thermal_shutdown_check(&dev.thset);
    if (THSET_STATE_2 == get_dev_thset()->state) {
        change_state(PM_STATE_OVERHEAT);
    }
    switch (pm->pmState) {
    case PM_STATE_INIT:
        struct_powermon_init(pm);
        if (enable_power) {
            update_power_switches(pm, true);
            change_state(PM_STATE_WAITINPUT);
        } else {
            change_state(PM_STATE_OFF);
        }
        break;
    case PM_STATE_WAITINPUT:
        if (!enable_power) {
            change_state(PM_STATE_OFF);
            break;
        }
        if (input_power_normal || power_critical_ok) {
            change_state(PM_STATE_RAMP);
            break;
        }
        if (stateTicks() > INIT_TIMEOUT_TICKS) {
            log_put(LOG_NOTICE, "No input power");
            change_state(PM_STATE_OFF);
        }
        break;
    case PM_STATE_STANDBY:
        if (enable_power && input_power_normal) {
            update_power_switches(pm, true);
            change_state(PM_STATE_RAMP);
        }
        break;
    case PM_STATE_RAMP:
        if (!enable_power) {
            change_state(PM_STATE_OFF);
            break;
        }
        if (power_critical_ok) {
            log_put(LOG_NOTICE, "Critical power supplies ready");
            change_state(PM_STATE_RUN);
        }
        if (stateTicks() > RAMP_TIMEOUT_TICKS) {
            log_put(LOG_ERR, "Critical power supplies failure");
            change_state(PM_STATE_PWRFAIL);
        }
        break;
    case PM_STATE_RUN:
        if (!enable_power) {
            change_state(PM_STATE_OFF);
            break;
        }
        if (input_power_critical) {
            log_put(LOG_ERR, "Input power lost");
            change_state(PM_STATE_OFF);
            break;
        }
        if (power_critical_failure) {
            log_put(LOG_ERR, "Critical power supplies failure");
            change_state(PM_STATE_PWRFAIL);
            break;
        }
        if (pm->monState != MON_STATE_READ) {
            log_put(LOG_ERR, "Error in STATE_RUN");
            change_state(PM_STATE_ERROR);
            break;
        }
        if (TEST_RESTART && (stateTicks() > 5000)) {
            change_state(PM_STATE_OFF);
            break;
        }
        break;

    case PM_STATE_OVERHEAT:
        if (!enable_power) {
            clear_thermal_shutdown(get_dev_thset());
            change_state(PM_STATE_OFF);
            break;
        }
        if (THSET_STATE_0 == get_dev_thset()->state)
            change_state(PM_STATE_STANDBY);
        break;
    case PM_STATE_PWRFAIL:
        update_power_switches(pm, false);
        change_state(PM_STATE_FAILWAIT);
        // dev_switchPower(&dev.pm, false);
        break;
    case PM_STATE_FAILWAIT:
        if (stateTicks() > POWERFAIL_DELAY_TICKS) {
            change_state(PM_STATE_STANDBY);
        }
        break;
    case PM_STATE_ERROR:
//        dev_switchPower(&dev.pm, false);
        if (stateTicks() > ERROR_DELAY_TICKS) {
            change_state(PM_STATE_OFF);
        }
        break;
    case PM_STATE_OFF:
        update_power_switches(pm, false);
        change_state(PM_STATE_STANDBY);
        break;
    }

    for (int i=0; i<POWERMON_SENSORS; i++)
        pm->sensors[i].rampState = (pm->pmState == PM_STATE_RAMP) ? RAMP_UP : RAMP_NONE;

    if (!enable_power)
        monClearMinMax(pm);

    uint32_t ticks = osKernelSysTick() - sensorReadTick;
    uint32_t interval = (pm->pmState == PM_STATE_RAMP) ? sensor_read_interval_ramp : sensor_read_interval_run;
    if (ticks > interval) {
        sensorReadTick = osKernelSysTick();
        runMon(pm);
    }

    update_system_powergood_pin(pm->sensors);
    if ((pm->pmState == PM_STATE_RAMP)
            || (pm->pmState == PM_STATE_RUN)
            ) {
        log_sensor_status_change(pm);
    } else {
        clearOldSensorStatus();
    }
    dev_thset_run(get_dev_thset());
}
