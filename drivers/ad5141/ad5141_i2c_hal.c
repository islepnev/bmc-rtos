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

#include "ad5141_i2c_hal.h"

#include "bsp.h"
#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 10;

bool ad5141_detect(BusInterface *bus)
{
    int Trials = 2;
    return i2c_driver_detect(bus, Trials, I2C_TIMEOUT_MS);
}

bool ad5141_write(BusInterface *bus, uint8_t ctrl_addr, uint8_t data)
{
    return i2c_driver_mem_write8(bus, ctrl_addr, &data, 1, I2C_TIMEOUT_MS);
}

bool ad5141_read(BusInterface *bus, uint16_t command, uint8_t *data)
{
    return i2c_driver_mem_read16(bus, command, data, 1, I2C_TIMEOUT_MS);
}
