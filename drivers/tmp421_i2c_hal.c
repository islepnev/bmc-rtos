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

#include "tmp421_i2c_hal.h"

#include "i2c.h"
#include "bsp.h"

static const int I2C_TIMEOUT_MS = 50;

enum { TMP421_I2C_ADDRESS = 0x1c };

HAL_StatusTypeDef tmp421_detect(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hi2c_sensors, TMP421_I2C_ADDRESS << 1,
                                Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef tmp421_read(Dev_temp *d)
{
    HAL_StatusTypeDef ret;
    uint8_t data1;
    uint8_t data2;
    uint16_t addr = (TMP421_I2C_ADDRESS << 1)|1;
    ret = HAL_I2C_Mem_Read(hi2c_sensors, addr, 0x01, I2C_MEMADD_SIZE_8BIT,
                           &data1, 1, I2C_TIMEOUT_MS);
    if (ret != HAL_OK)
        return ret;
    ret = HAL_I2C_Mem_Read(hi2c_sensors, addr, 0x11, I2C_MEMADD_SIZE_8BIT,
                           &data2, 1, I2C_TIMEOUT_MS);
    if (d) {
        d->valid = ret == HAL_OK;
        if(d->valid) {
            d->rawTemp = (int16_t) (data1<<8 |data2);
            d->temp = 1.*d->rawTemp/256;
        }
    }
    return ret;
}
