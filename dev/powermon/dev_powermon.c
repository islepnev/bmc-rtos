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

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bsp_powermon.h"
#include "bsp_powermon_types.h"
#include "bsp_sensors_config.h"
#include "cmsis_os.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "dev_powermon_types.h"
#include "devicelist.h"
#include "gpio.h"
#include "log/log.h"

void create_sensor_subdevices(Dev_powermon *d)
{
    pm_sensors_arr *sensors = &d->priv.sensors;
    bsp_pm_sensors_arr_init(sensors);
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *sensor = &sensors->arr[i];
        BusInterface bus = sensor->dev.bus;
        create_device(&d->dev, &sensor->dev, &sensor->priv, DEV_CLASS_INA226, bus, sensor->priv.label);
        sensors->count = i+1;
    }
}

void struct_powermon_sensors_clear(Dev_powermon_priv *p)
{
    pm_sensors_arr *sensors = &p->sensors;
    for (int i=0; i<sensors->count; i++) {
        struct_pm_sensor_clear(&sensors->arr[i]);
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

bool get_critical_power_valid(const pm_sensors_arr *sensors)
{
    if (0 == sensors->count)
        return false;
    for (int i=0; i < sensors->count; i++) {
        const pm_sensor *sensor = &sensors->arr[i];
        if (!sensor->priv.isOptional) {
            SensorStatus s = pm_sensor_status(sensor);
            if (s == SENSOR_UNKNOWN || s == SENSOR_CRITICAL)
                return false;
        }
    }
    return true;
}

bool get_critical_power_failure(const pm_sensors_arr *sensors)
{
    for (int i=0; i < sensors->count; i++) {
        const pm_sensor *sensor = &sensors->arr[i];
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

//bool pm_sensors_isAllValid(const Dev_powermon *d)
//{
//    const pm_sensors_arr *sensors = &d->priv.sensors;
//    for (int i=0; i < sensors->count; i++)
//        if (!pm_sensor_isValid(&sensors->arr[i]))
//            return false;
//    return true;
//}

bool get_fpga_core_power_present(const pm_sensors_arr *sensors)
{
    SensorStatus status_1v0 = pm_sensor_status(&sensors->arr[SENSOR_FPGA_CORE_1V0]);
    SensorStatus status_1v8 = pm_sensor_status(&sensors->arr[SENSOR_FPGA_1V8]);
    bool present_1v0 = ((status_1v0 == SENSOR_NORMAL) || (status_1v0 == SENSOR_WARNING));
    bool present_1v8 = ((status_1v8 == SENSOR_NORMAL) || (status_1v8 == SENSOR_WARNING));
    return present_1v0 && present_1v8;
}
