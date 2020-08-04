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

#include "digipot/dev_digipot_types.h"

#include "bsp_digipot.h"
#include "devicelist.h"

void create_digipots_subdevices(Dev_digipots *d)
{
    d->priv.count = 0;
    for (int i=0; i<MAX_DIGIPOT_COUNT; i++) {
        Dev_ad5141 zz = {0};
        Dev_ad5141 *pot = &d->priv.pot[i];
        *pot = zz;
        pot->priv.index = i;
        pot->dev.parent = &d->dev;
        pot->dev.device_status = DEVICE_UNKNOWN;
        pot->priv.sensorIndex = potSensorIndex(i);
        BusInterface bus_info = {
            .type = BUS_IIC,
            .bus_number = potBusNumber(i),
            .address = potBusAddress(i)
        };
        create_device(&d->dev, &pot->dev, &pot->priv, DEV_CLASS_AD5141, bus_info, potLabel(i));
        d->priv.count++;
    }
}
