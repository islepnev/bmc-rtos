/*
**    Copyright 2019 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "i2c_util.h"
#include <stdio.h>
#include "stm32f7xx_hal.h"

static const int I2C_TIMEOUT_MS = 10;

void i2c_scan(I2C_HandleTypeDef *i2c)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    for (int i=0; i<128; i++) {
        __HAL_I2C_DISABLE(i2c);
        __HAL_I2C_ENABLE(i2c);
        ret = HAL_I2C_IsDeviceReady(i2c, i << 1, Trials, I2C_TIMEOUT_MS);
        if (ret == HAL_TIMEOUT) {
            continue;
        }
        if (ret == HAL_OK) {
            printf("Found %02X\n", i);
            continue;
        }
        // HAL error
        printf("I2C HAL error\n");
        break;
    }
    HAL_Delay(1000);
}
