/*
**    Copyright 2019-2021 Ilja Slepnev
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

#ifndef BSP_DIGIPOT_H
#define BSP_DIGIPOT_H

#include "bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POT_TDC_A,
    POT_TDC_B
} PotIndex;

enum {DEV_DIGIPOT_COUNT = 2};

const char *potLabel(PotIndex index);
int potBusNumber(PotIndex index);
int potBusAddress(PotIndex index);
int potSensorIndex(PotIndex index);

#ifdef __cplusplus
}
#endif

#endif // BSP_DIGIPOT_H
