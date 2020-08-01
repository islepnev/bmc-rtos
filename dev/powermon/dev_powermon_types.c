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

#include "dev_powermon_types.h"

#include "bsp_powermon.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"

SensorStatus pm_sensors_getStatus(const Dev_powermon_priv *priv)
{
    SensorStatus maxStatus = SENSOR_NORMAL;
    const pm_sensors_arr *sensors = &priv->sensors;
    for (int i=0; i < sensors->count; i++) {
        const pm_sensor *sensor = &sensors->arr[i];
        if (sensor->priv.isOptional)
            continue;
        DeviceStatus deviceStatus = sensor->dev.device_status;
        if (deviceStatus != DEVICE_NORMAL)
            maxStatus = SENSOR_CRITICAL;
        int isOn = monIsOn(priv->sw, (SensorIndex)i);
        if (isOn) {
            SensorStatus status = pm_sensor_status(sensor);
            if (status > maxStatus)
                maxStatus = status;
        }
    }
    return maxStatus;
}

SensorStatus getMonStatus(const Dev_powermon_priv *priv)
{
    SensorStatus monStatus = SENSOR_CRITICAL;
    if ((priv->monState == MON_STATE_READ)
//            && (getMonStateTicks(pm) > SENSORS_SETTLE_TICKS)
            ) {
        monStatus = pm_sensors_getStatus(priv);
    }
    return monStatus;
}

SensorStatus getPowermonStatus(void)
{
    const Dev_powermon_priv *priv = get_powermon_priv_const();
    if (!priv)
        return SENSOR_UNKNOWN;
    const SensorStatus monStatus = getMonStatus(priv);
    const PmState pmState = priv->pmState;
    SensorStatus pmStatus = (pmState == PM_STATE_RUN || pmState == PM_STATE_OFF) ? SENSOR_NORMAL : SENSOR_WARNING;
    if (pmState == PM_STATE_PWRFAIL || pmState == PM_STATE_OVERHEAT || pmState == PM_STATE_ERROR)
        pmStatus = SENSOR_CRITICAL;
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (pmStatus > systemStatus)
        systemStatus = pmStatus;
    if (monStatus > systemStatus)
        systemStatus = monStatus;
    return systemStatus;
}

Dev_powermon_priv *get_powermon_priv(void)
{
    DeviceBase *d = find_device(DEV_CLASS_POWERMON);
    if (!d || !d->priv)
        return 0;
    return (Dev_powermon_priv *)device_priv(d);
}

const Dev_powermon_priv *get_powermon_priv_const(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_POWERMON);
    if (!d || !d->priv)
        return 0;
    return (const Dev_powermon_priv *)device_priv_const(d);
}

PmState get_powermon_state(void)
{
    const Dev_powermon_priv *priv = get_powermon_priv_const();
    return priv ? priv->pmState : PM_STATE_OFF;
}
