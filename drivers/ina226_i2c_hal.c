//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "ina226_i2c_hal.h"
#include "i2c.h"

static const int I2C_TIMEOUT_MS = 10;

HAL_StatusTypeDef ina226_i2c_Detect(uint16_t deviceAddr)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(&hi2c4, deviceAddr << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ina226_i2c_Read(uint16_t deviceAddr, uint16_t reg, uint16_t *data)
{
    HAL_StatusTypeDef ret;
    int Size = 2;
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c4, deviceAddr << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint16_t)pData[0] << 8) | pData[1];
        }
    }
    return ret;
}
