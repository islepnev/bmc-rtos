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
#include "bsp_i2c_callbacks.h"

#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_i2c.h"
#include "bsp.h"
#include "powermon_i2c_driver.h"
#include "pll_i2c_driver.h"

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_I2C_MasterTxCpltCallback();
    if (hi2c == hi2c_sensors)
        powermon_I2C_MasterTxCpltCallback();
}

void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_I2C_MasterRxCpltCallback();
    if (hi2c == hi2c_sensors)
        powermon_I2C_MasterRxCpltCallback();
}

void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_HAL_I2C_MemTxCpltCallback();
    if (hi2c == hi2c_sensors)
        powermon_HAL_I2C_MemTxCpltCallback();
}

void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_HAL_I2C_MemRxCpltCallback();
    if (hi2c == hi2c_sensors)
        powermon_HAL_I2C_MemRxCpltCallback();
}

void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_HAL_I2C_ErrorCallback();
    if (hi2c == hi2c_sensors)
        powermon_HAL_I2C_ErrorCallback();
}

void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == hPll)
        pll_HAL_I2C_AbortCpltCallback();
    if (hi2c == hi2c_sensors)
        powermon_HAL_I2C_AbortCpltCallback();
}
