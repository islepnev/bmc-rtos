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

#include "app_shared_data.h"
#include "bsp.h"
#include "bsp_powermon.h"
#include "dev_common_types.h"
#include "log/log.h"
#include "powermon/dev_pm_sensors.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_types.h"
#include "thset/dev_thset.h"

static const int TEST_RESTART = 0; // debug only
static const uint32_t INIT_TIMEOUT_TICKS = 3000;
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

static void log_sensor_status(const pm_sensor *p)
{
    const pm_sensor_priv *sensor = &p->priv;
    const SensorStatus status = pm_sensor_status(p);
    enum { size = 50 };
    static char str[size];
    if (p->dev.sensor != SENSOR_UNKNOWN)
        snprintf(str, size, "%s %s, %.3f V, %.3f A",
                 sensor->label,
                 sensor_status_text(status),
                 sensor->busVoltage,
                 sensor->current
                 );
    else
        snprintf(str, size, "%s %s",
                 sensor->label,
                 sensor_status_text(status)
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
}

static void log_sensors_change(const Dev_powermon *pm)
{
    const pm_sensors_arr *sensors = &pm->priv.sensors;
    for (int i=0; i<sensors->count; i++) {
        const pm_sensor *sensor = &sensors->arr[i];
        if (sensor->priv.rampState != RAMP_NONE)
            continue;
        SensorStatus status = pm_sensor_status(sensor);
        if (status != oldSensorStatus[i]) {
            log_sensor_status(sensor);
            oldSensorStatus[i] = status;
        }
    }
}

static void log_critical_sensors(const Dev_powermon_priv *p)
{
    const pm_sensors_arr *sensors = &p->sensors;
    for (int i=0; i<sensors->count; i++) {
        const pm_sensor *sensor = &sensors->arr[i];
        if (sensor->priv.isOptional)
            continue;
        log_sensor_status(sensor);
    }
}

static int pm_initialized = 0;
static bool old_inut_power_normal = false;
static bool old_inut_power_critical = false;

void powermon_task_init(void)
{
    clearOldSensorStatus();
}

static void change_state(Dev_powermon_priv *p, PmState state)
{
    if (p->pmState == state)
        return;
    p->pmState = state;
    stateStartTick = osKernelSysTick();
}

void task_powermon_run (Dev_powermon *pm)
{
    Dev_powermon_priv *priv = &pm->priv;
    if (!pm_initialized) {
        powermon_task_init();
        pm_initialized = 1;
    }
    pmLoopCount++;
    pm_sensors_arr *sensors = &pm->priv.sensors;
    bool vmePresent = pm_read_liveInsert(priv);
    pm_read_pgood(priv->pgood);
    const bool input_power_normal = get_input_power_normal(sensors);
    if (input_power_normal != old_inut_power_normal) {
        if (input_power_normal)
            log_put(LOG_NOTICE, "Input power normal");
        old_inut_power_normal = input_power_normal;
    }
    const int input_power_critical = get_input_power_failed(sensors);
    if (input_power_critical != old_inut_power_critical) {
        if (input_power_critical) {
            log_critical_sensors(priv);
            log_put(LOG_WARNING, "Input power critical");
        }
        old_inut_power_critical = input_power_critical;
    }
    const int power_critical_ok = get_critical_power_valid(sensors);
    const int power_critical_failure = get_critical_power_failure(sensors);

//    const thset_state_t thset_state = thermal_shutdown_check(&dev.thset);
    if (THSET_STATE_2 == get_thset_state()) {
        change_state(priv, PM_STATE_OVERHEAT);
    }
    switch (priv->pmState) {
    case PM_STATE_INIT:
        pm_clear_all(priv);
        if (enable_power) {
            update_power_switches(priv, true);
            change_state(priv, PM_STATE_WAITINPUT);
        } else {
            change_state(priv, PM_STATE_OFF);
        }
        break;
    case PM_STATE_WAITINPUT:
        if (!enable_power) {
            change_state(priv, PM_STATE_OFF);
            break;
        }
        if (input_power_normal) {
            change_state(priv, PM_STATE_RAMP);
            break;
        }
        if (stateTicks() > INIT_TIMEOUT_TICKS) {
            log_put(LOG_NOTICE, "No input power");
            change_state(priv, PM_STATE_OFF);
        }
        break;
    case PM_STATE_STANDBY:
        if (enable_power && input_power_normal) {
            update_power_switches(priv, true);
            change_state(priv, PM_STATE_RAMP);
        }
        break;
    case PM_STATE_RAMP:
        if (!enable_power) {
            change_state(priv, PM_STATE_OFF);
            break;
        }
        if (power_critical_ok) {
            log_put(LOG_NOTICE, "Critical power supplies ready");
            change_state(priv, PM_STATE_RUN);
        }
        if (stateTicks() > RAMP_TIMEOUT_TICKS) {
            log_put(LOG_ERR, "Critical power supplies failure");
            change_state(priv, PM_STATE_PWRFAIL);
        }
        break;
    case PM_STATE_RUN:
        if (!enable_power) {
            change_state(priv, PM_STATE_OFF);
            break;
        }
        if (input_power_critical) {
            log_put(LOG_ERR, "Input power lost");
            change_state(priv, PM_STATE_OFF);
            break;
        }
        if (power_critical_failure) {
            log_put(LOG_ERR, "Critical power supplies failure");
            change_state(priv, PM_STATE_PWRFAIL);
            break;
        }
        if (priv->monState != MON_STATE_READ) {
            log_put(LOG_ERR, "Error in STATE_RUN");
            change_state(priv, PM_STATE_ERROR);
            break;
        }
        if (TEST_RESTART && (stateTicks() > 5000)) {
            change_state(priv, PM_STATE_OFF);
            break;
        }
        break;

    case PM_STATE_OVERHEAT:
        if (!enable_power) {
            clear_thermal_shutdown();
            change_state(priv, PM_STATE_OFF);
            break;
        }
        if (THSET_STATE_0 == get_thset_state())
            change_state(priv, PM_STATE_STANDBY);
        break;
    case PM_STATE_PWRFAIL:
        update_power_switches(priv, false);
        change_state(priv, PM_STATE_FAILWAIT);
        // dev_switchPower(&dev.pm, false);
        break;
    case PM_STATE_FAILWAIT:
        if (stateTicks() > POWERFAIL_DELAY_TICKS) {
            change_state(priv, PM_STATE_STANDBY);
        }
        break;
    case PM_STATE_ERROR:
//        dev_switchPower(&dev.pm, false);
        if (stateTicks() > ERROR_DELAY_TICKS) {
            change_state(priv, PM_STATE_OFF);
        }
        break;
    case PM_STATE_OFF:
        update_power_switches(priv, false);
        change_state(priv, PM_STATE_STANDBY);
        break;
    }

    for (int i=0; i<POWERMON_SENSORS; i++)
        sensors->arr[i].priv.rampState = (priv->pmState == PM_STATE_RAMP) ? RAMP_UP : RAMP_NONE;

    if (!enable_power)
        monClearMinMax(pm);

    uint32_t ticks = osKernelSysTick() - sensorReadTick;
    uint32_t interval = (priv->pmState == PM_STATE_RAMP) ? sensor_read_interval_ramp : sensor_read_interval_run;
    if (ticks > interval) {
        sensorReadTick = osKernelSysTick();
        runMon(pm);
    }

    system_power_present = get_critical_power_valid(sensors);
    bsp_update_system_powergood_pin(system_power_present);
    if ((priv->pmState == PM_STATE_RAMP)
            || (priv->pmState == PM_STATE_RUN)
            ) {
        log_sensors_change(pm);
    } else {
        clearOldSensorStatus();
    }
}
