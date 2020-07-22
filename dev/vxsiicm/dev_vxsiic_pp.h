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
#ifndef DEV_VXSIIC_PP_H
#define DEV_VXSIIC_PP_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"
#include "dev_vxsiicm_types.h"

bool dev_vxsiic_read_pp(Dev_vxsiicm *d, int pp);
//HAL_StatusTypeDef dev_vxsiic_read_pp_eeprom(Dev_vxsiicm *d, int pp);
//HAL_StatusTypeDef dev_vxsiic_read_pp_ioexp(Dev_vxsiicm *d, int pp);
//HAL_StatusTypeDef dev_vxsiic_read_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t *data);
//HAL_StatusTypeDef dev_vxsiic_write_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t data);

#endif // DEV_VXSIIC_PP_H
