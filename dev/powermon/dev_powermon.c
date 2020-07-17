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

#include "dev_powermon.h"

#include "app_shared_data.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bsp_powermon_types.h"
#include "bsp_powermon.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "logbuffer.h"
#include "powermon_i2c_driver.h"

#include "cmsis_os.h"
#include "gpio.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 1000;

void struct_powermon_sensors_init(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_init(&d->sensors[i], (SensorIndex)(i));
    }
}

void struct_powermon_init(Dev_powermon *d)
{
    d->monState = MON_STATE_INIT;
    d->stateStartTick = 0;
    d->monErrors = 0;
    d->monCycle = 0;
    struct_powermon_sensors_init(d);
//    d->present = DEVICE_UNKNOWN;
    d->vmePresent = 0;
    init_pgood(d->pgood);
    init_power_switches(d->sw);
}

bool pm_read_liveInsert(Dev_powermon *pm)
{
    pm->vmePresent = readLiveInsertPin();
    return pm->vmePresent;
}

bool get_critical_power_valid(const pm_sensors_arr sensors)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &sensors[i];
        if (!sensor->isOptional)
            if (!pm_sensor_isValid(sensor))
                return false;
    }
    return true;
}

bool get_critical_power_failure(const pm_sensors_arr sensors)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &sensors[i];
        if (!sensor->isOptional)
            if (pm_sensor_isCritical(sensor))
                return true;
    }
    return false;
}

static bool check_power_switches(const Dev_powermon *pm)
{
    bool ret = true;
    for (int i=0; i<POWER_SWITCH_COUNT; i++) {
        if (pm->sw_state[i] != pm->sw[i]) {
            log_printf(LOG_CRIT, "%s switch failure: stuck %s",
                       psw_label(i), pm->sw_state[i] ? "high" : "low");
            ret = false;
        }
    }
    return ret;
}

bool update_power_switches(Dev_powermon *pm, bool state)
{
    if (state)
        log_put(LOG_NOTICE, "Switching ON");
    else
        log_put(LOG_NOTICE, "Switching OFF");
    switch_power(pm, state);
    read_power_switches_state(pm->sw_state);
    bool ok = pm_switches_isEqual(pm->sw_state, pm->sw);
    check_power_switches(pm);
    return ok;
}

bool pm_sensors_isAllValid(const Dev_powermon *d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(&d->sensors[i]))
            return false;
    return true;
}

void monClearMinMax(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        struct_pm_sensor_clear_minmax(&d->sensors[i]);
}

void monClearMeasurements(Dev_powermon *d)
{
    monClearMinMax(d);
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_measurements(&d->sensors[i]);
    }
}

int monDetect(Dev_powermon *d)
{
    int count = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        powermon_i2c_reset_master();
        DeviceStatus s = pm_sensor_detect(&d->sensors[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            powermon_i2c_reset_master();
        }
    }
    return count;
}

int monReadValues(Dev_powermon *d)
{
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *sensor = &d->sensors[i];
        if (sensor->deviceStatus == DEVICE_NORMAL) {
            DeviceStatus s = pm_sensor_read(sensor);
            if (s != DEVICE_NORMAL)
                err++;
        }
    }
    return err;
}

void pm_setStateStartTick(Dev_powermon *pm)
{
    pm->stateStartTick = osKernelSysTick();
}

uint32_t getMonStateTicks(const Dev_powermon *pm)
{
    return osKernelSysTick() - pm->stateStartTick;
}

int old_num_detected = 0;
int stable_detect_count = 0;

MonState runMon(Dev_powermon *pm)
{
    pm->monCycle++;
    const MonState oldState = pm->monState;
    switch(pm->monState) {
    case MON_STATE_INIT:
        monClearMeasurements(pm);
        pm->monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT: {
        const int num_detected = monDetect(pm);
        if (num_detected == 0) {
            pm->monState = MON_STATE_INIT;
            break;
        }
        if (num_detected > 0 && (old_num_detected == num_detected))
            stable_detect_count++;
        else
            stable_detect_count = 0;
        old_num_detected = num_detected;
        if (stable_detect_count >= 2) {
            update_board_version(num_detected);
            log_printf(LOG_INFO, "%d sensors detected", num_detected);
            pm->monState = MON_STATE_READ;
            break;
        }

        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d sensors found", num_detected);
            pm->monState = MON_STATE_READ;
        }
        break;
    }
    case MON_STATE_READ:
        if (monReadValues(pm) == 0)
            pm->monState = MON_STATE_READ;
        else
            pm->monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        log_put(LOG_ERR, "Sensor read error");
        pm->monErrors++;
        pm->monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
    if (oldState != pm->monState) {
        pm_setStateStartTick(pm);
    }
    return pm->monState;
}

bool get_fpga_core_power_present(const pm_sensors_arr sensors)
{
    SensorStatus status_1v0 = pm_sensor_status(&sensors[SENSOR_FPGA_CORE_1V0]);
    SensorStatus status_1v8 = pm_sensor_status(&sensors[SENSOR_FPGA_1V8]);
    bool present_1v0 = ((status_1v0 == SENSOR_NORMAL) || (status_1v0 == SENSOR_WARNING));
    bool present_1v8 = ((status_1v8 == SENSOR_NORMAL) || (status_1v8 == SENSOR_WARNING));
    return present_1v0 && present_1v8;
}
