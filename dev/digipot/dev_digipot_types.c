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

#include "bsp_digipot.h"

void struct_pots_init(Dev_digipots *d)
{
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        Dev_ad5141 zz = {0};
        d->pot[i] = zz;
        d->pot[i].index = i;
        d->pot[i].deviceStatus = DEVICE_UNKNOWN;
        d->pot[i].sensorIndex = potSensorIndex(i);
        d->pot[i].bus.type = BUS_IIC;
        d->pot[i].bus.bus_number = potBusNumber(i);
        d->pot[i].bus.address = potBusAddress(i);
    }
}

SensorStatus get_digipot_sensor_status(const Dev_digipots *d)
{
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        if (d->pot[i].deviceStatus != DEVICE_NORMAL)
            return SENSOR_CRITICAL;
    }
    return SENSOR_NORMAL;
}
