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
#include "bsp_powermon.h"
#include "devices_types.h"
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
    d->pgood.pgood_1v0_core = 0;
    d->pgood.pgood_1v0_mgt = 0;
    d->pgood.pgood_1v2_mgt = 0;
    d->pgood.pgood_2v5 = 0;
    d->pgood.pgood_3v3 = 0;
    d->pgood.pgood_3v3_fmc = 0;
    d->sw.switch_5v = 1;
    d->sw.switch_3v3 = 1;
    d->sw.switch_2v5 = 1;
    d->sw.switch_1v0_core = 1;
    d->sw.switch_1v0_mgt = 1;
    d->sw.switch_1v2_mgt = 1;
    d->sw.switch_5v_fmc = 1;
}

static int readLiveInsertPin(void)
{
    bool state = true; // TODO: read_gpio_pin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (false == state);
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

void update_system_powergood_pin(const pm_sensors_arr sensors)
{
    system_power_present = get_critical_power_valid(sensors);
    write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, system_power_present);
}

bool pm_switches_isEqual(const pm_switches l, const pm_switches r)
{
    return l.switch_5v == r.switch_5v
           && l.switch_5v_fmc == r.switch_5v_fmc
           && l.switch_3v3 == r.switch_3v3
           && l.switch_2v5 == r.switch_2v5
           && l.switch_1v0_core == r.switch_1v0_core
           && l.switch_1v0_mgt == r.switch_1v0_mgt
           && l.switch_1v2_mgt == r.switch_1v2_mgt;
}

static bool check_power_switches(const Dev_powermon *pm)
{
    bool ret = true;
    if (pm->sw_state.switch_5v != pm->sw.switch_5v) {
        log_printf(LOG_CRIT, "5V switch failure: stuck %s", pm->sw_state.switch_5v ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_3v3 != pm->sw.switch_3v3) {
        log_printf(LOG_CRIT, "3.3V switch failure: stuck %s", pm->sw_state.switch_3v3 ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_2v5 != pm->sw.switch_2v5) {
        log_printf(LOG_CRIT, "2.5V switch failure: stuck %s", pm->sw_state.switch_2v5 ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_1v0_core != pm->sw.switch_1v0_core) {
        log_printf(LOG_CRIT, "1.0V-core switch failure: stuck %s", pm->sw_state.switch_1v0_core ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_1v0_mgt != pm->sw.switch_1v0_mgt) {
        log_printf(LOG_CRIT, "1.0V-MGT switch failure: stuck %s", pm->sw_state.switch_1v0_mgt ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_5v_fmc != pm->sw.switch_5v_fmc) {
        log_printf(LOG_CRIT, "5V-FMC switch failure: stuck %s", pm->sw_state.switch_5v_fmc ? "high" : "low");
        ret = false;
    }

    return ret;
}

bool update_power_switches(Dev_powermon *pm, bool state)
{
    // int pcb_ver = get_mcb_pcb_ver();
    if (state)
        log_put(LOG_NOTICE, "Switching ON");
    else
        log_put(LOG_NOTICE, "Switching OFF");
    pm->sw.switch_1v0_core = state;
    pm->sw.switch_1v0_mgt = state;
    pm->sw.switch_1v2_mgt = state;
    pm->sw.switch_2v5 = state;
    pm->sw.switch_3v3 = state;
    pm->sw.switch_5v_fmc = state;
    pm->sw.switch_5v = 1; // (pcb_ver == PCB_VER_A_MCB_1_0) ? 1 : state; // TTVXS version
    write_power_switches(&pm->sw);
    if (state)
        osDelay(1); // allow 20 us for charge with pullups
//    pm->sw_state
    read_power_switches_state(&pm->sw_state);
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
        powermon_i2c_reset_master();
        int num_detected = monDetect(pm);
        if (num_detected == 0) {
            pm->monState = MON_STATE_INIT;
            break;
        }
        if (num_detected == POWERMON_SENSORS) {
            log_printf(LOG_INFO, "All %d sensors present", num_detected);
            pm->monState = MON_STATE_READ;
            break;
        }
        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d of %d found", num_detected, POWERMON_SENSORS);
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

static double get_sensor_power_w(const pm_sensor *d)
{
    SensorStatus sensor_status = pm_sensor_status(d);
    int sensor_present = ((sensor_status == SENSOR_NORMAL) || (sensor_status == SENSOR_WARNING));
    if (sensor_present)
        return d->busVoltage * d->current;
    else
        return 0;
}

double pm_get_power_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += get_sensor_power_w(&pm->sensors[SENSOR_5VPC]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_VXS_5V]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_MCB_4V5]);
    return mw;
}

double pm_get_power_max_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += pm->sensors[SENSOR_5VPC].powerMax;
    mw += pm->sensors[SENSOR_VXS_5V].powerMax;
    mw += pm->sensors[SENSOR_MCB_4V5].powerMax;
    return mw;
}

bool get_fpga_core_power_present(const pm_sensors_arr sensors)
{
    SensorStatus status_1v0 = pm_sensor_status(&sensors[SENSOR_FPGA_CORE_1V0]);
    SensorStatus status_1v8 = pm_sensor_status(&sensors[SENSOR_FPGA_1V8]);
    bool present_1v0 = ((status_1v0 == SENSOR_NORMAL) || (status_1v0 == SENSOR_WARNING));
    bool present_1v8 = ((status_1v8 == SENSOR_NORMAL) || (status_1v8 == SENSOR_WARNING));
    return present_1v0 && present_1v8;
}
