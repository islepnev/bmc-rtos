/*
**    Digital Potentiometers
**
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

#include "dev_digipot_types.h"

#include "devicelist.h"

SensorStatus get_digipot_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_DIGIPOTS);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;
    const Dev_digipots_priv *priv = (const Dev_digipots_priv *)device_priv_const(d);
    for (unsigned int i=0; i<priv->count; i++) {
        if (priv->pot[i].dev.device_status != DEVICE_NORMAL)
            return SENSOR_CRITICAL;
    }
    return SENSOR_NORMAL;
}
