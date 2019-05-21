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
#ifndef VXSIIC_HAL_H
#define VXSIIC_HAL_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __I2C_HandleTypeDef * const vxsiic_hi2c;

void vxsiic_init(void);
void vxsiic_reset_i2c_master(void);
void vxsiic_reset_mux(void);
HAL_StatusTypeDef vxsiic_detect_mux(void);
HAL_StatusTypeDef vxsiic_mux_select(uint8_t subdevice, uint8_t channel);
HAL_StatusTypeDef vxsiic_get_pp_i2c_status(uint8_t pp);
//HAL_StatusTypeDef vxsiic_detect_pp_eeprom(uint8_t pp);
HAL_StatusTypeDef vxsiic_read_pp_eeprom(uint8_t pp, uint16_t reg, uint8_t *data);
HAL_StatusTypeDef vxsiic_read_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t *data);
HAL_StatusTypeDef vxsiic_write_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t data);
void vxsiic_I2C_MasterTxCpltCallback(void);
void vxsiic_I2C_MasterRxCpltCallback(void);
void vxsiic_HAL_I2C_MemTxCpltCallback(void);
void vxsiic_HAL_I2C_MemRxCpltCallback(void);
void vxsiic_HAL_I2C_ErrorCallback(void);
void vxsiic_HAL_I2C_AbortCpltCallback(void);

#ifdef __cplusplus
}
#endif

#endif // VXSIIC_HAL_H
