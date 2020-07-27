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

#ifndef I2C_DRIVER_IMPL_H
#define I2C_DRIVER_IMPL_H

#include <stdbool.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __I2C_HandleTypeDef;

void i2c_driver_log_error(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress);
void i2c_driver_reset_internal(struct __I2C_HandleTypeDef *hi2c);
bool i2c_driver_detect_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t millisec);
bool i2c_driver_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec);
bool i2c_driver_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec);
bool i2c_driver_mem_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec);
bool i2c_driver_mem_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_IMPL_H
