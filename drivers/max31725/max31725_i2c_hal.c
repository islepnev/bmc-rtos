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

#include "max31725_i2c_hal.h"

#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 50;

bool max31725_detect(BusInterface *bus)
{
    uint32_t Trials = 2;
    return i2c_driver_detect(bus, Trials, I2C_TIMEOUT_MS);
}

bool max31725_read(BusInterface *bus, uint16_t reg, uint16_t *data)
{
    int Size = 2;
    uint8_t pData[Size];
    if (! i2c_driver_mem_read8(bus, reg, pData, Size, I2C_TIMEOUT_MS))
        return false;
    if (data) {
        *data = ((uint16_t)pData[0] << 8) | pData[1];
    }
    return true;
}
