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

#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <stdint.h>

typedef enum {
    SENSOR_DISCRETE  = 0,
    SENSOR_TEMPERATURE = 1,
    SENSOR_VOLTAGE = 2,
    SENSOR_CURRENT = 3,
} SensorType;

enum { SENSOR_NAME_SIZE = 16 };

typedef struct GenericSensor {
    uint8_t type;
    uint8_t state;
    double value;
    char name[SENSOR_NAME_SIZE];
} GenericSensor;

enum { MAX_SENSOR_COUNT = 32 };

#endif // SENSOR_TYPES_H
