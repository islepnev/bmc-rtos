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

#ifndef DEV_VXSIIC_H
#define DEV_VXSIIC_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"
#include "dev_common_types.h"
#include "dev_vxsiic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//HAL_StatusTypeDef vxsiic_detect(void);
//HAL_StatusTypeDef vxsiic_read(int slot, uint8_t address, uint8_t *data);
DeviceStatus dev_vxsiic_detect(Dev_vxsiic *d);
HAL_StatusTypeDef dev_vxsiic_read(Dev_vxsiic *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIIC_H
