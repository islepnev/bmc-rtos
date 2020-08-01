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

#include "dev_pm_sensors_util.h"

#include "dev_pm_sensors.h"
#include "log/log.h"
#include "cmsis_os.h"

void monClearMinMax(Dev_powermon *d)
{
    pm_sensors_arr *sensors = &d->priv.sensors;
    for (int i=0; i<sensors->count; i++)
        struct_pm_sensor_clear_minmax(&sensors->arr[i].priv);
}

void monClearMeasurements(Dev_powermon *d)
{
    monClearMinMax(d);
    pm_sensors_arr *sensors = &d->priv.sensors;
    for (int i=0; i<sensors->count; i++) {
        struct_pm_sensor_clear_measurements(&sensors->arr[i].priv);
    }
}

int monDetect(Dev_powermon *d)
{
    int count = 0;
    int errors = 0;
    pm_sensors_arr *sensors = &d->priv.sensors;
    for (int i=0; i<sensors->count; i++) {
        if (errors > 2) break;
        DeviceStatus s = pm_sensor_detect(&sensors->arr[i]);
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
    pm_sensors_arr *sensors = &d->priv.sensors;
    for (int i=0; i<sensors->count; i++) {
        pm_sensor *sensor = &sensors->arr[i];
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
