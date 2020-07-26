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

#include "dev_max31725.h"

#include "app_shared_data.h"
#include "devices_types.h"
#include "max31725/max31725_i2c_hal.h"

void dev_max31725_init(BusInterface *bus)
{
    Dev_max31725 *d = get_dev_max31725();
    d->bus = *bus;
}

bool dev_max31725_detect(Dev_max31725 *d)
{
    return max31725_detect(&d->bus);
}

bool dev_max31725_read(Dev_max31725 *d)
{
    uint16_t data;
    if (! max31725_read(&d->bus, 0, &data))
        return false;
    d->temp = 1. * data/256+64;
    return true;
}
