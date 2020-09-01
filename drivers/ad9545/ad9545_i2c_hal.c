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

#include "ad9545_i2c_hal.h"

#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 100;

static bool pll_i2c_detect(BusInterface *bus, uint32_t Trials)
{
    return i2c_driver_detect(bus, Trials, I2C_TIMEOUT_MS);
}

static bool pll_i2c_mem_read(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_read16(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}

static bool pll_i2c_mem_write(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_write16(bus, MemAddress, pData, Size, I2C_TIMEOUT_MS);
}

bool ad9545_i2c_detect(BusInterface *bus)
{
    uint32_t Trials = 2;
    return pll_i2c_detect(bus, Trials);
}

bool ad9545_read1(BusInterface *bus, uint16_t reg, uint8_t *data)
{
    enum {Size = 1};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg, pData, Size))
        return false;
    if (data) {
        *data = pData[0];
    }
    return true;
}

bool ad9545_write1(BusInterface *bus, uint16_t reg, uint8_t data)
{
    int Size = 1;
    uint8_t pData[Size];
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_read2(BusInterface *bus, uint16_t reg, uint16_t *data)
{
    enum {Size = 2};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg, pData, Size))
        return false;
    if (data) {
        *data = ((uint32_t)pData[1] << 8) | pData[0];
    }
    return true;
}

bool ad9545_write2(BusInterface *bus, uint16_t reg, uint16_t data)
{
    int Size = 2;
    uint8_t pData[Size];
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_read3(BusInterface *bus, uint16_t reg, uint32_t *data)
{
    enum {Size = 3};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg, pData, Size))
        return false;
    if (data) {
        *data = ((uint32_t)pData[2] << 16) | ((uint32_t)pData[1] << 8) | pData[0];
    }
    return true;
}

bool ad9545_write3(BusInterface *bus, uint16_t reg, uint32_t data)
{
    int Size = 3;
    uint8_t pData[Size];
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_read4(BusInterface *bus, uint16_t reg, uint32_t *data)
{
    enum {Size = 4};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg , pData, Size))
        return false;
    if (data) {
        *data = ((uint32_t)pData[3] << 24)
                | ((uint32_t)pData[2] << 16)
                | ((uint32_t)pData[1] << 8)
                | pData[0];
    }
    return true;
}

bool ad9545_write4(BusInterface *bus, uint16_t reg, uint32_t data)
{
    enum {Size = 4};
    uint8_t pData[Size];
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_read5(BusInterface *bus, uint16_t reg, uint64_t *data)
{
    enum {Size = 5};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg , pData, Size))
        return false;
    if (data) {
        *data = ((uint64_t)pData[4] << 32)
                | ((uint32_t)pData[3] << 24)
                | ((uint32_t)pData[2] << 16)
                | ((uint16_t)pData[1] << 8)
                | pData[0];
    }
    return true;
}

bool ad9545_write5(BusInterface *bus, uint16_t reg, uint64_t data)
{
    enum {Size = 5};
    uint8_t pData[Size];
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_read6(BusInterface *bus, uint16_t reg, uint64_t *data)
{
    enum {Size = 6};
    uint8_t pData[Size];
    if (! pll_i2c_mem_read(bus, reg , pData, Size))
        return false;
    if (data) {
        *data = ((uint64_t)pData[5] << 40)
                | ((uint64_t)pData[4] << 32)
                | ((uint32_t)pData[3] << 24)
                | ((uint32_t)pData[2] << 16)
                | ((uint16_t)pData[1] << 8)
                | pData[0];
    }
    return true;
}

bool ad9545_write6(BusInterface *bus, uint16_t reg, uint64_t data)
{
    enum {Size = 6};
    uint8_t pData[Size];
    pData[5] = (data >> 40) & 0xFF;
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}

bool ad9545_write8(BusInterface *bus, uint16_t reg, uint64_t data)
{
    enum {Size = 8};
    uint8_t pData[Size];
    pData[7] = (data >> 56) & 0xFF;
    pData[6] = (data >> 48) & 0xFF;
    pData[5] = (data >> 40) & 0xFF;
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    return pll_i2c_mem_write(bus, reg, pData, Size);
}
