/*
**    Generic interrupt mode SPI driver
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

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __I2C_HandleTypeDef;

int hi2c_index(struct __I2C_HandleTypeDef *hi2c);
void i2c_driver_init(void);
void i2c_driver_reset(struct __I2C_HandleTypeDef *handle);
HAL_StatusTypeDef i2c_driver_detect(struct __I2C_HandleTypeDef *hi2c, uint16_t deviceAddr, uint32_t Trials, uint32_t millisec);
HAL_StatusTypeDef i2c_driver_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec);
HAL_StatusTypeDef i2c_driver_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec);
HAL_StatusTypeDef i2c_driver_mem_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec);
HAL_StatusTypeDef i2c_driver_mem_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec);

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_H
