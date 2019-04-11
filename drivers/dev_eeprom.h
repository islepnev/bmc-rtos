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

#ifndef DEV_EEPROM_H
#define DEV_EEPROM_H

#include <unistd.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef dev_eepromVxsPb_Detect(void);
HAL_StatusTypeDef at24c_detect(void);
HAL_StatusTypeDef dev_eepromVxsPb_Read(uint16_t addr, uint8_t *data);
HAL_StatusTypeDef at24c_read(uint16_t addr, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // DEV_EEPROM_H
