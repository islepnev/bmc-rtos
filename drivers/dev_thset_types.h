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
#ifndef DEV_THSET_TYPES_H
#define DEV_THSET_TYPES_H

#include <stdint.h>

enum {DEV_THERM_COUNT = 4};

typedef struct Dev_adt7301 {
    int valid;
    int16_t rawTemp;
} Dev_adt7301;

typedef enum {
    THSET_STATE_0,
    THSET_STATE_1,
    THSET_STATE_2,
} thset_state_t;

typedef struct Dev_thset {
    thset_state_t state;
    Dev_adt7301 th[DEV_THERM_COUNT];
} Dev_thset;

#endif // DEV_THSET_TYPES_H
