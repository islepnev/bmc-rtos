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
#ifndef DEV_POT_H
#define DEV_POT_H

#include <stdint.h>
#include "dev_types.h"

enum {DEV_POT_COUNT = 3};

typedef struct Dev_ad5141 {
    uint8_t busAddress;
    DeviceStatus deviceStatus;
    int16_t value;
} Dev_ad5141;

typedef struct Dev_pots {
    Dev_ad5141 pot[DEV_POT_COUNT];
} Dev_pots;

void struct_pots_init(Dev_pots *d);
int pot_detect(Dev_pots *d);

#endif // DEV_POT_H
