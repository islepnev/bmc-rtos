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
#include "dev_eeprom_types.h"
#include "stm32f7xx_hal.h"
#include "i2c.h"

static const int eeprom_Config_busAddress = 0x50;
static I2C_HandleTypeDef * const hi2c = &hi2c3;

static const int I2C_TIMEOUT_MS = 10;

static void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

static HAL_StatusTypeDef at24c_detect(uint8_t address)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 10;
    ret = HAL_I2C_IsDeviceReady(hi2c, address << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef at24c_read(uint8_t address, uint16_t addr, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hi2c, address << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    struct_at24c_init(d);
    if (HAL_OK == at24c_detect(eeprom_Config_busAddress))
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (HAL_OK == at24c_read(eeprom_Config_busAddress, 0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

DeviceStatus dev_eepromConfig_read(Dev_at24c *d)
{
    uint8_t data = 0;
    if (HAL_OK != at24c_read(eeprom_Config_busAddress, 0, &data)) {
        d->present = DEVICE_FAIL;
    }
    return d->present;
}

