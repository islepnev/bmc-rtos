/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "dev_powermon.h"

#include "app_shared_data.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bsp_powermon.h"
#include "bsp_powermon_types.h"
#include "cmsis_os.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "dev_powermon_types.h"
#include "gpio.h"
#include "logbuffer.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 1000;

void create_sensor_subdevices(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *sensor = &d->priv.sensors[i];
        struct_pm_sensor_init(sensor, (SensorIndex)(i));
        BusInterface bus_info = {
            .type = BUS_IIC,
            .bus_number = sensorBusNumber(i),
            .address = sensorBusAddress(i)
        };
        create_device(&d->dev, &sensor->dev, &sensor->priv, DEV_CLASS_INA226, bus_info, sensor->priv.label);
    }
}

void struct_powermon_sensors_clear(Dev_powermon_priv *p)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear(&p->sensors[i]);
    }
}

void pm_clear_all(Dev_powermon_priv *p)
{
    p->monState = MON_STATE_INIT;
    p->stateStartTick = 0;
    p->monErrors = 0;
    p->monCycle = 0;
    struct_powermon_sensors_clear(p);
    //    d->present = DEVICE_UNKNOWN;
    p->vmePresent = 0;
    init_pgood(p->pgood);
    init_power_switches(p->sw);
}

bool pm_read_liveInsert(Dev_powermon_priv *p)
{
    p->vmePresent = readLiveInsertPin();
    return p->vmePresent;
}

bool get_critical_power_valid(const pm_sensors_arr sensors)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &sensors[i];
        if (!sensor->priv.isOptional)
            if (!pm_sensor_isValid(sensor))
                return false;
    }
    return true;
}

bool get_critical_power_failure(const pm_sensors_arr sensors)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &sensors[i];
        if (!sensor->priv.isOptional)
            if (pm_sensor_isCritical(sensor))
                return true;
    }
    return false;
}

static bool check_power_switches(const Dev_powermon_priv *p)
{
    bool ret = true;
    for (int i=0; i<POWER_SWITCH_COUNT; i++) {
        if (p->sw_state[i] != p->sw[i]) {
            log_printf(LOG_CRIT, "%s switch failure: stuck %s",
                       psw_label((PowerSwitchIndex)i), p->sw_state[i] ? "high" : "low");
            ret = false;
        }
    }
    return ret;
}

bool update_power_switches(Dev_powermon_priv *p, bool state)
{
    if (state)
        log_put(LOG_NOTICE, "Switching ON");
    else
        log_put(LOG_NOTICE, "Switching OFF");
    switch_power(p, state);
    read_power_switches_state(p->sw_state);
    bool ok = pm_switches_isEqual(p->sw_state, p->sw);
    check_power_switches(p);
    return ok;
}

bool pm_sensors_isAllValid(const Dev_powermon *d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(&d->priv.sensors[i]))
            return false;
    return true;
}

void monClearMinMax(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        struct_pm_sensor_clear_minmax(&d->priv.sensors[i].priv);
}

void monClearMeasurements(Dev_powermon *d)
{
    monClearMinMax(d);
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_measurements(&d->priv.sensors[i].priv);
    }
}

int monDetect(Dev_powermon *d)
{
    int count = 0;
    int errors = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        if (errors > 2) break;
        DeviceStatus s = pm_sensor_detect(&d->priv.sensors[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            errors++;
        }
    }
    return count;
}

int monReadValues(Dev_powermon *d)
{
    int ok = 0;
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *sensor = &d->priv.sensors[i];
        if (err >= 2) {
            pm_sensor_set_sensorStatus(sensor, SENSOR_UNKNOWN);
        } else {
            if (sensor->dev.device_status == DEVICE_NORMAL) {
                DeviceStatus s = pm_sensor_read(sensor);
                if (s == DEVICE_NORMAL)
                    ok++;
                else
                    err++;
            }
        }
    }
    if (ok == 0) {
        log_put(LOG_ERR, "No SMBus sensors read");
    }
    return err;
}

void pm_setStateStartTick(Dev_powermon *pm)
{
    pm->priv.stateStartTick = osKernelSysTick();
}

uint32_t getMonStateTicks(const Dev_powermon *pm)
{
    return osKernelSysTick() - pm->priv.stateStartTick;
}

int old_num_detected = 0;
int stable_detect_count = 0;

MonState runMon(Dev_powermon *pm)
{
    pm->priv.monCycle++;
    const MonState oldState = pm->priv.monState;
    switch(pm->priv.monState) {
    case MON_STATE_INIT:
        monClearMeasurements(pm);
        pm->priv.monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT: {
        const int num_detected = monDetect(pm);
        if (num_detected == 0) {
            pm->priv.monState = MON_STATE_INIT;
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
            pm->priv.monState = MON_STATE_READ;
            break;
        }

        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d sensors found", num_detected);
            pm->priv.monState = MON_STATE_READ;
        }
        break;
    }
    case MON_STATE_READ:
        if (monReadValues(pm) == 0)
            pm->priv.monState = MON_STATE_READ;
        else
            pm->priv.monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        log_put(LOG_ERR, "Sensor read error");
        pm->priv.monErrors++;
        pm->priv.monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
    if (oldState != pm->priv.monState) {
        pm_setStateStartTick(pm);
    }
    return pm->priv.monState;
}

bool get_fpga_core_power_present(const pm_sensors_arr sensors)
{
    SensorStatus status_1v0 = pm_sensor_status(&sensors[SENSOR_FPGA_CORE_1V0]);
    SensorStatus status_1v8 = pm_sensor_status(&sensors[SENSOR_FPGA_1V8]);
    bool present_1v0 = ((status_1v0 == SENSOR_NORMAL) || (status_1v0 == SENSOR_WARNING));
    bool present_1v8 = ((status_1v8 == SENSOR_NORMAL) || (status_1v8 == SENSOR_WARNING));
    return present_1v0 && present_1v8;
}
