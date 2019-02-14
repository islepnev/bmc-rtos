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

#include "dev_eeprom.h"
#include "stm32f7xx_hal.h"
#include "i2c.h"

HAL_StatusTypeDef dev_eepromVxsPb_Read(uint16_t addr, uint8_t *data)
{
    const int eepromVxsPbDeviceAddr = 0x50;
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c2, eepromVxsPbDeviceAddr << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, 100);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef dev_eepromConfig_Read(uint16_t addr, uint8_t *data)
{
    const int eepromConfigDeviceAddr = 0x51;
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c1, eepromConfigDeviceAddr << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, 100);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}
