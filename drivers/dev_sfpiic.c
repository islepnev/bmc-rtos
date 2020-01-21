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
#include "dev_sfpiic_types.h"

#include "i2c.h"
#include "bsp.h"
#include "bsp_sfpiic.h"
#include "bus/i2c_driver.h"
#include "cmsis_os.h"

static const int I2C_TIMEOUT_MS = 10;

enum { PCA9548_BASE_I2C_ADDRESS = 0x74 };

static HAL_StatusTypeDef sfp_i2c_detect(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint32_t Trials = 2;
    ret = i2c_driver_detect(&hi2c_sfpiic, PCA9548_BASE_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

static HAL_StatusTypeDef sfp_i2c_read(int slot, uint8_t address, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    // TODO
    return ret;
}

static HAL_StatusTypeDef pca9548_read(uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t pData;
    ret = i2c_driver_read(&hi2c_sfpiic, PCA9548_BASE_I2C_ADDRESS << 1, &pData, 1, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData;
        }
    }
    return ret;
}

DeviceStatus dev_sfpiic_detect(Dev_sfpiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    bsp_sfpiic_reset();
    if (HAL_OK != sfp_i2c_detect())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

DeviceStatus dev_sfpiic_read(Dev_sfpiic *d)
{
    uint8_t data = 0;
    if (HAL_OK != pca9548_read(&data))
        d->present = DEVICE_FAIL;
    return d->present;
}

