/*
**    Generic interrupt mode IIC driver
**
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

#ifndef I2C_DRIVER_CALLBACKS_H
#define I2C_DRIVER_CALLBACKS_H

#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __I2C_HandleTypeDef;

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_CALLBACKS_H
