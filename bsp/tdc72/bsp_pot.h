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

#ifndef BSP_POT_H
#define BSP_POT_H

#ifdef __cplusplus
extern "C" {
#endif

enum {DEV_POT_COUNT = 3};

typedef enum {
    POT_TDC_A,
    POT_TDC_B,
    POT_TDC_C
} PotIndex;

const char *potLabel(PotIndex index);
int potBusAddress(PotIndex index);
int potSensorIndex(PotIndex index);

#ifdef __cplusplus
}
#endif

#endif // BSP_POT_H
