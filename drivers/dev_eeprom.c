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

static const int eeprom_VxsPb_busAddress = 0x51;
static const int eeprom_Config_busAddress = 0x50;
//I2C_HandleTypeDef *hi2c_eeprom_VxsPb = &hi2c1;
I2C_HandleTypeDef *hi2c_eeprom_Config = &hi2c2;

static const int I2C_TIMEOUT_MS = 10;

HAL_StatusTypeDef dev_eepromVxsPb_Detect(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint32_t Trials = 2;
//    ret = HAL_I2C_IsDeviceReady(hi2c_eeprom_VxsPb, eeprom_VxsPb_busAddress << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef at24c_detect(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hi2c_eeprom_Config, eeprom_Config_busAddress << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef dev_eepromVxsPb_Read(uint16_t addr, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    enum {Size = 1};
    uint8_t pData[Size];
//    ret = HAL_I2C_Mem_Read(hi2c_eeprom_VxsPb, eeprom_VxsPb_busAddress << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef at24c_read(uint16_t addr, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
//    enum {Size = 1};
//    uint8_t pData[Size];
//    ret = HAL_I2C_Mem_Read(hi2c_eeprom_Config, eeprom_Config_busAddress << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
//    if (ret == HAL_OK) {
//        if (data) {
//            *data = pData[0];
//        }
//    }
    return ret;
}
