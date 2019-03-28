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

#include "dev_sfpiic.h"

#include "stm32f7xx_hal.h"
#include "i2c.h"

static const int I2C_TIMEOUT_MS = 10;

static I2C_HandleTypeDef * const hi2c = &hi2c2;

enum { PCA9548_BASE_I2C_ADDRESS = 0x74 };

static HAL_StatusTypeDef sfp_i2c_detect(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hi2c, PCA9548_BASE_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

static HAL_StatusTypeDef sfp_i2c_read(int slot, uint8_t address, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    return ret;
}

static HAL_StatusTypeDef pca9548_read(uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t pData;
    ret = HAL_I2C_Master_Receive(hi2c, PCA9548_BASE_I2C_ADDRESS << 1, &pData, 1, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData;
        }
    }
    return ret;
}

static void dev_sfpiic_reset(void)
{
    HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  GPIO_PIN_SET);
}

DeviceStatus dev_sfpiic_detect(Dev_sfpiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    dev_sfpiic_reset();
    if (HAL_OK != sfp_i2c_detect())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

HAL_StatusTypeDef dev_sfpiic_read(Dev_sfpiic *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t data = 0;
    ret = pca9548_read(&data);
    return ret;
}

