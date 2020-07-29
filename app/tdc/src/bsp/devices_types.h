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

//#include "dev_eeprom_vxspb.h"
#include "dev_thset_types.h"
#include "dev_sfpiic_types.h"
#include "digipot/dev_digipot_types.h"

typedef struct Devices {
    Dev_thset thset;
    Dev_sfpiic sfpiic;
    // Dev_eeprom_vxspb eeprom_vxspb;
    Dev_digipots pots;
    uint32_t pcb_ver;
} Devices;

#endif // DEVICES_TYPES_H
