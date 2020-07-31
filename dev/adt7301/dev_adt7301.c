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


#include "dev_adt7301.h"

#include "i2c.h"
#include "adt7301/adt7301_spi_hal.h"


bool dev_adt7301_detect(Dev_adt7301 *d)
{
    int16_t data;
    if (adt7301_read(&d->dev.bus, &data))
        return true;
    return false;
}

bool dev_adt7301_read(Dev_adt7301 *d)
{
    int16_t data;
    if (! adt7301_read(&d->dev.bus, &data)) {
        return false;
    }
    int16_t rawTemp = (int16_t)data;
    d->priv.temp = (double)rawTemp/32.0;
    return true;
}
