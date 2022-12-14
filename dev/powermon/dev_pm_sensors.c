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

#include "dev_pm_sensors.h"

#include <assert.h>
#include <math.h>

#include "ansi_escape_codes.h"
#include "cmsis_os.h"
#include "dev_mcu.h"
#include "bsp_sensors_config.h"
#include "dev_pm_sensors_types.h"
#include "dev_pm_sensors_util.h"
#include "device_status_log.h"
#include "display.h"
#include "ina226/ina226_i2c_hal.h"
#include "log/log.h"

//const double SENSOR_VOLTAGE_MARGIN_WARN = 0.05;
const double SENSOR_VOLTAGE_MARGIN_CRIT = 0.1;

void struct_pm_sensor_clear_minmax(pm_sensor_priv *d)
{
    d->busVoltageMin = 0;
    d->busVoltageMax = 0;
    d->currentMin = 0;
    d->currentMax = 0;
    d->powerMax = 0;
}

void struct_pm_sensor_clear_measurements(pm_sensor_priv *d)
{
    d->valid = false;
    d->busVoltage = 0;
    d->current = 0;
    d->power = 0;
}

void struct_pm_sensor_clear(pm_sensor *d)
{
    pm_sensor_priv *sensor = &d->priv;
    sensor->rampState = RAMP_NONE;
    sensor->lastStatusUpdatedTick = 0;
    const double current_max = 16; // amperers
    sensor->current_lsb = current_max / 32768.0;
    sensor->cal = 0.00512 / (sensor->current_lsb * fabs(sensor->shuntVal));
    struct_pm_sensor_clear_measurements(&d->priv);
    struct_pm_sensor_clear_minmax(&d->priv);
}


uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor_priv *d)
{
    return osKernelSysTick() - d->lastStatusUpdatedTick;
}


SensorStatus pm_sensor_compute_status(const pm_sensor *d)
{
    if (d->dev.device_status != DEVICE_NORMAL ||
        !d->priv.valid) {
        return SENSOR_UNKNOWN;
    }
    const pm_sensor_priv *sensor = &d->priv;
    double V = d->priv.busVoltage;
    double VMinWarn = sensor->busNomVoltage * (1-sensor->voltageMarginWarn);
    double VMaxWarn = sensor->busNomVoltage * (1+sensor->voltageMarginWarn);
    double VMinCrit = sensor->busNomVoltage * (1-sensor->voltageMarginCrit);
    double VMaxCrit = sensor->busNomVoltage * (1+sensor->voltageMarginCrit);
    int VNorm = (V > VMinWarn) && (V < VMaxWarn);
    int VWarn = (V > VMinCrit) && (V < VMaxCrit);
    if (VNorm && VWarn) {
        return SENSOR_NORMAL;
    }
    if (VWarn) {
        return SENSOR_WARNING;
    }
    return SENSOR_CRITICAL;
}

double get_sensor_power_w(const pm_sensor *d)
{
    SensorStatus sensor_status = pm_sensor_status(d);
    int sensor_present = ((sensor_status == SENSOR_NORMAL) || (sensor_status == SENSOR_WARNING));
    if (sensor_present)
        return d->priv.busVoltage * d->priv.current;
    else
        return 0;
}

SensorStatus pm_sensor_status(const pm_sensor *d)
{
    if (d->dev.device_status != DEVICE_NORMAL)
        return SENSOR_UNKNOWN;
    return d->dev.sensor;
}

bool pm_sensor_isValid(const pm_sensor *d)
{
    return pm_sensor_isNormal(d) || pm_sensor_isWarning(d);
}

bool pm_sensor_isNormal(const pm_sensor *d)
{
    SensorStatus status = pm_sensor_status(d);
    return status == SENSOR_NORMAL;
}

bool pm_sensor_isWarning(const pm_sensor *d)
{
    SensorStatus status = pm_sensor_status(d);
    return status == SENSOR_WARNING;
}

bool pm_sensor_isCritical(const pm_sensor *d)
{
    SensorStatus status = pm_sensor_status(d);
    return status == SENSOR_CRITICAL;
}

