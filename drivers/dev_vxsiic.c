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

#include "dev_vxsiic.h"

#include "stm32f7xx_hal.h"
#include "i2c.h"
#include "dev_types.h"

static const int I2C_TIMEOUT_MS = 10;

static I2C_HandleTypeDef * const hi2c = &hi2c1;

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };

static HAL_StatusTypeDef vxsiic_detect(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    for (int i=0; i<3; i++) {
        ret = HAL_I2C_IsDeviceReady(hi2c, (PCA9548_BASE_I2C_ADDRESS + i) << 1, Trials, I2C_TIMEOUT_MS);
        if (ret != HAL_OK)
            return ret;
    }
    return ret;
}

//HAL_StatusTypeDef pca9548_read(uint8_t *data, int subdevice)
//{
//    HAL_StatusTypeDef ret;
//    uint8_t pData;
//    ret = HAL_I2C_Master_Receive(hi2c, (PCA9548_BASE_I2C_ADDRESS + subdevice) << 1, &pData, 1, I2C_TIMEOUT_MS);
//    if (ret == HAL_OK) {
//        if (data) {
//            *data = pData;
//        }
//    }
//    return ret;
//}

static HAL_StatusTypeDef vxsiic_select_slot(Dev_vxsiic *d, int slot)
{
    HAL_StatusTypeDef ret = HAL_OK;
    return ret;
}

static HAL_StatusTypeDef vxsiic_read_slot(Dev_vxsiic *d, int slot)
{
    HAL_StatusTypeDef ret = HAL_OK;
    return ret;
}

static void dev_vxsiic_reset(void)
{
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_SET);
}

DeviceStatus dev_vxsiic_detect(Dev_vxsiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    dev_vxsiic_reset();
    if (HAL_OK != vxsiic_detect())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

HAL_StatusTypeDef vxsiic_read(Dev_vxsiic *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int i=0; i<VXSIIC_SLOTS; i++) {
        ret = vxsiic_select_slot(d, i);
        if (HAL_OK != ret)
            break;
        ret = vxsiic_read_slot(d, i);
        if (HAL_OK != ret)
            break;
    }
    return ret;
}
