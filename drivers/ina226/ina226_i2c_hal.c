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

#include "ina226_i2c_hal.h"
#include "i2c.h"
#include "powermon_i2c_driver.h"

bool ina226_i2c_Detect(uint16_t deviceAddr)
{
    uint32_t Trials = 2;
    return powermon_i2c_detect(deviceAddr << 1, Trials);
}

bool ina226_i2c_Read(uint16_t deviceAddr, uint16_t reg, uint16_t *data)
{
    int Size = 2;
    uint8_t pData[Size];
    if (! powermon_i2c_mem_read(deviceAddr << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, Size))
        return false;
    if (data) {
        *data = ((uint16_t)pData[0] << 8) | pData[1];
    }
    return true;
}

bool ina226_i2c_Write(uint16_t deviceAddr, uint16_t reg, uint16_t data)
{
    int Size = 2;
    uint8_t pData[Size];
    pData[0] = (data >> 8) & 0xFF;
    pData[1] = data & 0xFF;
    return powermon_i2c_mem_write(deviceAddr << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, Size);
}
