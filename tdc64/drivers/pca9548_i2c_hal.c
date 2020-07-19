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

#include "pca9548_i2c_hal.h"

//#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//#include "main.h"
#include "i2c.h"

static const int I2C_TIMEOUT_MS = 10;

enum { PCA9548_I2C_ADDRESS = 0x74 };

HAL_StatusTypeDef pca9548_read(uint8_t *data)
{
    HAL_StatusTypeDef ret;
    uint8_t pData;
    ret = HAL_I2C_Master_Receive(&hi2c4, PCA9548_I2C_ADDRESS << 1, &pData, 1, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData;
        }
    }
    return ret;
}
