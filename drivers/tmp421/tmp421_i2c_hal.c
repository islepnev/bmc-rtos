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


#include "tmp421_i2c_hal.h"

#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 50;

bool tmp421_detect(BusInterface *bus)
{
    uint32_t Trials = 2;
    return i2c_driver_detect(bus, Trials, I2C_TIMEOUT_MS);
}

bool tmp421_read(BusInterface *bus, uint8_t reg, uint8_t *data)
{
    uint8_t tmp;
    if (! i2c_driver_mem_read8(bus, reg,
                              &tmp, 1, I2C_TIMEOUT_MS))
        return false;
    if (data)
        *data = tmp;
    return true;
}
