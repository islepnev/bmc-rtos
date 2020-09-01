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

#include "vxsiic_iic_driver.h"

#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 25;

bool vxsiic_bus_ready(BusInterface *bus)
{
    return i2c_driver_bus_ready(bus);
}

bool vxsiic_detect(BusInterface *bus, uint8_t Trials)
{
    return i2c_driver_detect(bus, Trials, I2C_TIMEOUT_MS);
}

bool vxsiic_read(BusInterface *bus, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_read(bus, pData, Size, I2C_TIMEOUT_MS);
}

bool vxsiic_write(BusInterface *bus, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_write(bus, pData, Size, I2C_TIMEOUT_MS);
}

bool vxsiic_mem_read8(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_read8(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}

bool vxsiic_mem_read16(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_read16(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}

bool vxsiic_mem_write8(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_write8(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}

bool vxsiic_mem_write16(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_write16(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}
