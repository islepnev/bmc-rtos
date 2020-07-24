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

// CRU16 board specific definitions

#ifndef DEV_THERM_CONFIG_H
#define DEV_THERM_CONFIG_H

#include "therm/dev_therm_types.h"
#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {THERM_SENSORS = 2};

typedef enum {
    THERM_VCXO,
    THERM_FPGA_DIE
} ThermIndex;

BusType thermBusType(ThermIndex index);
int thermBusIndex(ThermIndex index);
int thermBusAddress(ThermIndex index);
ThermType thermType(ThermIndex index);
const char *thermLabel(ThermIndex index);

#ifdef __cplusplus
}
#endif

#endif // DEV_THERM_CONFIG_H
