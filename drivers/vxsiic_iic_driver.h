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
#ifndef VXSIIC_IIC_DRIVER_H
#define VXSIIC_IIC_DRIVER_H

#include "stm32f7xx_hal_def.h"

#ifdef  __cplusplus
extern "C" {
#endif

extern struct __I2C_HandleTypeDef * const vxsiic_hi2c;

void vxsiic_I2C_MasterTxCpltCallback(void);
void vxsiic_I2C_MasterRxCpltCallback(void);
void vxsiic_HAL_I2C_MemTxCpltCallback(void);
void vxsiic_HAL_I2C_MemRxCpltCallback(void);
void vxsiic_HAL_I2C_ErrorCallback(void);
void vxsiic_HAL_I2C_AbortCpltCallback(void);

void vxsiic_reset_i2c_master(void);
void vxsiic_init(void);
HAL_StatusTypeDef vxsiic_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef vxsiic_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef vxsiic_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef vxsiic_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

#ifdef  __cplusplus
}
#endif

#endif // VXSIIC_IIC_DRIVER_H
