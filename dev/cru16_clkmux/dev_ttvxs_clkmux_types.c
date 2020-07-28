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

#include "dev_ttvxs_clkmux_types.h"

SensorStatus get_clkmux_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_CLKMUX);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;
    const Dev_ttvxs_clkmux_priv *priv = (const Dev_ttvxs_clkmux_priv *)device_priv_const(d);

    switch (d->device_status) {
    case DEVICE_NORMAL:
        return SENSOR_NORMAL;
    case DEVICE_FAIL:
        return SENSOR_CRITICAL;
    default:
        return SENSOR_UNKNOWN;
    }
}
