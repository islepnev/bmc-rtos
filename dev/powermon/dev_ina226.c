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

#include "dev_ina226.h"

#include "cmsis_os.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "device_status_log.h"
#include "ina226.h"
#include "log/log.h"

const int ERROR_COUNT_LIMIT = 3;

static void pm_sensor_set_deviceStatus(pm_sensor *d, DeviceStatus status)
{
    DeviceStatus oldStatus = d->dev.device_status;
    if (oldStatus != status) {
        d->dev.device_status = status;
        d->priv.lastStatusUpdatedTick = osKernelSysTick();
        if (status != DEVICE_NORMAL)
            dev_log_status_change(&d->dev);
    }
}

static void pm_sensor_set_readVoltage(pm_sensor_priv *d, double value)
{
    d->busVoltage = value;
    if (value > d->busVoltageMax)
        d->busVoltageMax = value;
    if (value < d->busVoltageMin)
        d->busVoltageMin = value;
}

static void pm_sensor_set_readCurrent(pm_sensor_priv *d, double value)
{
    d->current = value;
    if (value > d->currentMax)
        d->currentMax = value;
    if (value < d->currentMin)
        d->currentMin = value;
}

static void pm_sensor_set_readPower(pm_sensor_priv *d, double value)
{
    d->power = value;
    if (value > d->powerMax)
        d->powerMax = value;
}

void pm_sensor_set_sensorStatus(pm_sensor *d, SensorStatus status)
{
    SensorStatus oldStatus = d->dev.sensor;
    if (oldStatus != status) {
        if (false)
            sensor_log_status_change(&d->dev);
        d->dev.sensor = status;
        d->priv.lastStatusUpdatedTick = osKernelSysTick();
    }
}

bool dev_ina226_detect(pm_sensor *d)
{
    d->priv.lastStatusUpdatedTick = osKernelSysTick();
    int detected =(
        ina226_detect(&d->dev.bus)
        && ina226_write_conf(&d->dev.bus, d->priv.cal)
        );
    pm_sensor_set_deviceStatus(d, detected ? DEVICE_NORMAL : DEVICE_FAIL);
    return detected;
}

DeviceStatus dev_ina226_read(pm_sensor *d)
{
    ina226_rawdata_t rawdata = {0};
    int err = 0;
    while (1) {
        if (err > 0)
            log_printf(LOG_WARNING, "Sensor %s read error, retry %d", d->priv.label, err);
        if (ina226_read(&d->dev.bus, &rawdata))
            break;
        err++;
        if (err >= ERROR_COUNT_LIMIT) {
            pm_sensor_set_deviceStatus(d, DEVICE_FAIL);
            pm_sensor_set_sensorStatus(d, SENSOR_UNKNOWN);
            struct_pm_sensor_clear_measurements(&d->priv);
            return d->dev.device_status;
        }
    }
    const double readVoltage  = (int16_t)rawdata.rawVoltage * 1.25e-3;
    pm_sensor_set_readVoltage(&d->priv, readVoltage);
#if INA226_USE_INTERNAL_CALC
    pm_sensor_set_readCurrent(d, d->current_lsb * (int16_t)rawdata.rawCurrent);
    pm_sensor_set_readPower(d, (int16_t)rawdata.rawPower * d->current_lsb * 25.0);
#else
    const double shuntVoltage = (int16_t)rawdata.rawShuntVoltage * 2.5e-6;
    double readCurrent = 0;
    if (d->priv.hasShunt && d->priv.shuntVal > 1e-6)
        readCurrent = shuntVoltage / d->priv.shuntVal;
    pm_sensor_set_readCurrent(&d->priv, readCurrent);
    pm_sensor_set_readPower(&d->priv, readCurrent * readVoltage);
#endif
    d->priv.valid = true;
    pm_sensor_set_sensorStatus(d, pm_sensor_compute_status(d));
    return d->dev.device_status;
}
