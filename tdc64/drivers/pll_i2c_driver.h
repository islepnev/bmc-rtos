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
#ifndef PLL_I2C_DRIVER_H
#define PLL_I2C_DRIVER_H

#include "stm32f7xx_hal_def.h"

extern struct __I2C_HandleTypeDef * const hPll;

void pll_i2c_init(void);
void pll_i2c_reset_master(void);
HAL_StatusTypeDef pll_i2c_detect(uint16_t deviceAddr, uint32_t Trials);
HAL_StatusTypeDef pll_i2c_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef pll_i2c_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

void pll_I2C_MasterTxCpltCallback(void);
void pll_I2C_MasterRxCpltCallback(void);
void pll_HAL_I2C_MemTxCpltCallback(void);
void pll_HAL_I2C_MemRxCpltCallback(void);
void pll_HAL_I2C_ErrorCallback(void);
void pll_HAL_I2C_AbortCpltCallback(void);

#endif // PLL_I2C_DRIVER_H
