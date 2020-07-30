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

#ifndef DEVICES_TYPES_H
#define DEVICES_TYPES_H

#include <stdint.h>

typedef struct Dev_sdcard {
    int detect_b;
} Dev_sdcard;

enum {
    PCB_VER_A_MCB_1_0 = 3,
    PCB_VER_A_MCB_1_1 = 1,
};

typedef struct Devices {
    Dev_sdcard sd;
    int pen_b;
    uint32_t pcb_ver;
} Devices;

#endif // DEVICES_TYPES_H
