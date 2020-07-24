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
#include "powermon_i2c_driver.h"
#include "bsp.h"
#include "i2c.h"
#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 10;

void powermon_i2c_reset_master(void)
{
    i2c_driver_reset(&hi2c_sensors);
}

bool powermon_i2c_detect(uint16_t deviceAddr, uint32_t Trials)
{
    return i2c_driver_detect(&hi2c_sensors, deviceAddr, Trials, I2C_TIMEOUT_MS);
}

bool powermon_i2c_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_read(&hi2c_sensors, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
}

bool powermon_i2c_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_write(&hi2c_sensors, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
}
