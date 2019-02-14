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

#include "ad9545_i2c_hal.h"

#include "stm32f7xx_hal.h"
#include "i2c.h"

const int pllDeviceAddr = 0x4A;

HAL_StatusTypeDef pllSendByte(uint16_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 2};
    uint8_t pData[Size];
    pData[0] = (data >> 8) & 0xFF;
    pData[1] = data & 0xFF;
    ret = HAL_I2C_Master_Transmit(&hi2c2, pllDeviceAddr << 1, pData, Size, 100);
    return ret;
}

HAL_StatusTypeDef pllReceiveByte(uint32_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t pData[Size] = {0, 0, 0};
    ret = HAL_I2C_Master_Receive(&hi2c2, pllDeviceAddr << 1, pData, Size, 100);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[2] << 16) | ((uint32_t)pData[1] << 8) | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef pllReadRegister(uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c2, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, 100);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[2] << 16) | ((uint32_t)pData[1] << 8) | pData[0];
        }
    }
    return ret;
}
