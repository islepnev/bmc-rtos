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
#ifndef AD5141_I2C_HAL_H
#define AD5141_I2C_HAL_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"

HAL_StatusTypeDef dev_ad5141_nop(uint8_t deviceAddress);
HAL_StatusTypeDef dev_ad5141_copy_rdac_to_eeprom(uint8_t deviceAddress);
HAL_StatusTypeDef dev_ad5141_copy_eeprom_to_rdac(uint8_t deviceAddress);
HAL_StatusTypeDef dev_ad5141_write_rdac(uint8_t deviceAddress, uint8_t data);
HAL_StatusTypeDef dev_ad5141_read_rdac(uint8_t deviceAddress, uint8_t *data);

#endif // AD5141_I2C_HAL_H
