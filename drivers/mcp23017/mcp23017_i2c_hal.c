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

#include "mcp23017_i2c_hal.h"

#include "bus/i2c_driver.h"
#include "devicebase.h"

static const int I2C_TIMEOUT_MS = 10;

//enum { MCP23017_BASE_I2C_ADDRESS = 0x20 };

//static BusInterface bus = {
//    .type = BUS_IIC,
//    .bus_number = 4,
//    .address = MCP23017_BASE_I2C_ADDRESS
//};

bool mcp23017_detect(DeviceBase *dev)
{
    uint32_t Trials = 2;
    return i2c_driver_detect(&dev->bus, Trials, I2C_TIMEOUT_MS);
}

bool mcp23017_read(DeviceBase *dev, uint8_t reg, uint8_t *data)
{
    enum {Size = 1};
    uint8_t pData[Size];
    if (! i2c_driver_mem_read8(&dev->bus, reg, pData, Size, I2C_TIMEOUT_MS))
        return false;
    if (data) {
        *data = pData[0];
    }
    return true;
}

static bool mcp23017_write_internal(DeviceBase *dev, uint8_t reg, uint8_t data)
{
    enum {Size = 1};
    uint8_t pData[Size];
    pData[0] = data;
    return i2c_driver_mem_write8(&dev->bus, reg, pData, Size, I2C_TIMEOUT_MS);
}

bool mcp23017_write(DeviceBase *dev, uint8_t reg, uint8_t data)
{
    if (! mcp23017_write_internal(dev, reg, data))
        return false;
    uint8_t read = 0;
    if (! mcp23017_read(dev, reg, &read))
        return false;
    return data == read;
}
