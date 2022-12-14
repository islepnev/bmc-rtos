/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef BUS_TYPES_H
#define BUS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum  BusType {
    BUS_NONE,
    BUS_SPI,
    BUS_IIC
} BusType;

typedef uint16_t BusIndex;

typedef struct BusInterface {
    uint8_t type; // BusType
    uint8_t bus_number;
    BusIndex address;
    struct DeviceBase *dev;
} BusInterface;

extern const BusInterface null_bus_info;

#ifdef __cplusplus
}
#endif

#endif // BUS_TYPES_H
