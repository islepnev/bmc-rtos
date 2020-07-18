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

#include "dev_pm_sensors_types.h"

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

SensorStatus pm_sensor_status(const pm_sensor *d)
{
    if (d->deviceStatus != DEVICE_NORMAL) {
//        printf("%s: device unknown\n", d->label);
        return SENSOR_UNKNOWN;
    }

    double V = d->busVoltage;
    double VMinWarn = d->busNomVoltage * (1-monVoltageMarginWarn(d->index));
    double VMaxWarn = d->busNomVoltage * (1+monVoltageMarginWarn(d->index));
    double VMinCrit = d->busNomVoltage * (1-monVoltageMarginCrit(d->index));
    double VMaxCrit = d->busNomVoltage * (1+monVoltageMarginCrit(d->index));
    int VNorm = (V > VMinWarn) && (V < VMaxWarn);
    int VWarn = (V > VMinCrit) && (V < VMaxCrit);
    if (VNorm && VWarn)
        return SENSOR_NORMAL;
    if (VWarn)
        return SENSOR_WARNING;
//    int isOn = 1; // FIXME: monIsOn(deviceAddr);
//    return isOn ? inRange2 : 1;
    return SENSOR_CRITICAL;
}
