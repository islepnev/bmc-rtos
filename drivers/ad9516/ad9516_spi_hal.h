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

#ifndef AD9516_SPI_HAL_H
#define AD9516_SPI_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void ad9516_test_loop(void);
HAL_StatusTypeDef ad9516_read1(uint16_t reg, uint8_t *data);
HAL_StatusTypeDef ad9516_write1(uint16_t reg, uint8_t data);
HAL_StatusTypeDef ad9516_write_config(uint8_t data);
void ad9516_enable_interface(void);
void ad9516_disable_interface(void);

#ifdef __cplusplus
}
#endif

#endif // AD9516_SPI_HAL_H
