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
#ifndef BSP_I2C_CALLBACKS_H
#define BSP_I2C_CALLBACKS_H

struct __I2C_HandleTypeDef;

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c);

#endif // BSP_I2C_CALLBACKS_H
