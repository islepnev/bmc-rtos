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
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "debug_helpers.h"

static const int I2C_TIMEOUT_MS = 25;

void vxsiic_reset_i2c_master(void)
{
    i2c_driver_reset(vxsiic_hi2c);
}

HAL_StatusTypeDef vxsiic_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_read(vxsiic_hi2c, DevAddress, pData, Size, I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef vxsiic_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_write(vxsiic_hi2c, DevAddress, pData, Size, I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef vxsiic_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_read(vxsiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef vxsiic_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    return i2c_driver_mem_write(vxsiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
}
