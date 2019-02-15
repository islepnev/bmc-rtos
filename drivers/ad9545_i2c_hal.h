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
#ifndef AD9545_I2C_HAL_H
#define AD9545_I2C_HAL_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void pllSetStaticPins(void);
HAL_StatusTypeDef ad9545_detect(void);
//HAL_StatusTypeDef pllSendByte(uint16_t data);
//HAL_StatusTypeDef pllReceiveByte(uint32_t *data);
HAL_StatusTypeDef ad9545_read(uint16_t reg, uint32_t *data);

#ifdef __cplusplus
}
#endif

#endif // AD9545_I2C_HAL_H