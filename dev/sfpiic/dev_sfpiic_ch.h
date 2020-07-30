/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include <stdbool.h>
#include <stdint.h>
#include "stm32f7xx_hal_def.h"

#include "dev_common_types.h"
#include "dev_sfpiic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool dev_sfpiic_ch_update(Dev_sfpiic *d, uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIIC_PP_H
