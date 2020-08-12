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

#include "dev_at24c.h"

#include "bus/bus_types.h"
#include "bus/i2c_driver.h"
#include "bus/impl/i2c_driver_util.h" // FIXME: use index, not handle
#include "i2c.h"

static const int I2C_TIMEOUT_MS = 10;

bool at24c_detect(BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    uint32_t Trials = 2;
    return i2c_driver_detect(hi2c, DevAddress, Trials, I2C_TIMEOUT_MS);
}

bool at24c_read(BusInterface *bus, uint16_t addr, uint8_t *data)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    enum {Size = 1};
    uint8_t pData[Size];
    bool ret = i2c_driver_mem_read(hi2c, DevAddress, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}