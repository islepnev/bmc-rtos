//
//    Copyright 2019 Ilja Slepnev
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
//#include "i2c.h"

#include "dev_pm_sensors_types.h"
#include "dev_powermon.h"
//#include "ansi_escape_codes.h"
//#include "display.h"
#include "devices.h"
#include "dev_powermon_types.h"
#include "dev_thset.h"
//#include "dev_leds.h"
#include "logbuffer.h"
#include "app_shared_data.h"

static const int TEST_RESTART = 0; // debug only
static const uint32_t SENSORS_SETTLE_TICKS = 200;
static const uint32_t THERM_SETTLE_TICKS = 1000;
static const uint32_t RAMP_TIMEOUT_TICKS = 3000;
static const uint32_t POWERFAIL_DELAY_TICKS = 3000;
static const uint32_t ERROR_DELAY_TICKS = 3000;

static const uint32_t log_sensor_status_duration_ticks = 3000;

uint32_t pmLoopCount = 0;

//PmState pmState = PM_STATE_INIT;

static const uint32_t sensorReadInterval = 100;
static uint32_t sensorReadTick = 0;

static const uint32_t thermReadInterval = 300;
static uint32_t thermReadTick = 0;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static SensorStatus oldSensorStatus[POWERMON_SENSORS] = {0};
static void clearOldSensorStatus(void)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        oldSensorStatus[i] = SENSOR_NORMAL;
}

static const char *sensorStatusStr(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return "  UNKNOWN";
    case SENSOR_NORMAL:   return "  NORMAL";
    case SENSOR_WARNING:  return " WARNING";
    case SENSOR_CRITICAL: return "CRITICAL";
    default: return "FAIL";
    }
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
                     sensorStatusStr(status),
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
//                if (pm_sensor_get_sensorStatus_Duration(sensor) >= log_sensor_status_duration_ticks)
                    log_put(LOG_INFO, str);
                break;
            }
            oldSensorStatus[i] = status;
        }
    }
}

static int pm_initialized = 0;

void task_powermon_run (void)
{
    Dev_powermon *pm = get_dev_powermon();
    if (!pm_initialized) {
        clearOldSensorStatus();
        pm_initialized = 1;
    }
    pmLoopCount++;
    int vmePresent = 1; // pm_read_liveInsert(&dev.pm);
    const PmState oldState = pm->pmState; // pmState;
    pm_read_pgood(pm);
    int power_input_ok = get_input_power_valid(pm);
    int power_critical_ok = get_critical_power_valid(pm);
    switch (pm->pmState) {
    case PM_STATE_INIT:
        struct_powermon_init(pm);
        pm->pmState = PM_STATE_STANDBY;
        break;
    case PM_STATE_STANDBY:
        if (vmePresent && enable_power) {
            if (power_input_ok) {
                log_put(LOG_NOTICE, "Input power Ok");
                pm->pmState = PM_STATE_RAMP;
            }
        }
        break;
    case PM_STATE_RAMP:
        if (!vmePresent || !enable_power || !power_input_ok) {
            pm->pmState = PM_STATE_STANDBY;
            break;
        }
        if (power_critical_ok) {
            log_put(LOG_NOTICE, "Critical power supplies ready");
            pm->pmState = PM_STATE_RUN;
        }
        if (stateTicks() > RAMP_TIMEOUT_TICKS) {
            if (!pm->pgood_3v3) {
                log_put(LOG_ERR, "3.3V power failure");
            }
            log_put(LOG_ERR, "Critical power supplies failure");
            pm->pmState = PM_STATE_PWRFAIL;
        }
        break;
    case PM_STATE_RUN:
        if (!vmePresent || !enable_power || !power_input_ok) {
            pm->pmState = PM_STATE_STANDBY;
            break;
        }
        if (!power_critical_ok) {
            log_put(LOG_ERR, "Critical power supplies failure");
            pm->pmState = PM_STATE_PWRFAIL;
            break;
        }
        if (pm->monState != MON_STATE_READ) {
            log_put(LOG_ERR, "Error in STATE_RUN");
            pm->pmState = PM_STATE_ERROR;
            break;
        }
        if (TEST_RESTART && (stateTicks() > 5000)) {
            pm->pmState = PM_STATE_STANDBY;
            break;
        }
        break;
    case PM_STATE_PWRFAIL:
//        dev_switchPower(&dev.pm, SWITCH_OFF);
        if (stateTicks() > POWERFAIL_DELAY_TICKS) {
            pm->pmState = PM_STATE_STANDBY;
        }
        break;
    case PM_STATE_ERROR:
//        dev_switchPower(&dev.pm, SWITCH_OFF);
        if (stateTicks() > ERROR_DELAY_TICKS) {
            pm->pmState = PM_STATE_STANDBY;
        }
        break;
    }

    if ((pm->pmState == PM_STATE_RAMP)
            || (pm->pmState == PM_STATE_RUN)) {
        update_power_switches(pm, SWITCH_ON);
    } else {
        update_power_switches(pm, SWITCH_OFF);
    }

    for (int i=0; i<POWERMON_SENSORS; i++)
        pm->sensors[i].rampState = (pm->pmState == PM_STATE_RAMP) ? RAMP_UP : RAMP_NONE;

    if ((pm->pmState == PM_STATE_STANDBY)
            || (pm->pmState == PM_STATE_RAMP)) {
        runMon(pm);
    } else {
        if (pm->pmState == PM_STATE_RUN) {
            uint32_t ticks = osKernelSysTick() - sensorReadTick;
            if (ticks > sensorReadInterval) {
                sensorReadTick = osKernelSysTick();
                runMon(pm);
            }

        }
    }
    update_system_powergood_pin(pm);
    if ((pm->pmState == PM_STATE_RAMP)
            || (pm->pmState == PM_STATE_RUN)
            ) {
        log_sensor_status_change(pm);
    } else {
        clearOldSensorStatus();
    }
    if ((pm->pmState == PM_STATE_RUN)
            && (getMonStateTicks(pm) > THERM_SETTLE_TICKS)) {
        uint32_t ticks = osKernelSysTick() - thermReadTick;
        if (ticks > thermReadInterval) {
            thermReadTick = osKernelSysTick();
            dev_thset_read(get_dev_thset());
        }
    } else {
        struct_thset_init(get_dev_thset());
    }

    if (oldState != pm->pmState) {
        stateStartTick = osKernelSysTick();
    }
}
