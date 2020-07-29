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

#ifndef IPMI_SENSOR_TYPES_H
#define IPMI_SENSOR_TYPES_H

#include <stdint.h>

typedef enum {
    IPMI_SENSOR_DISCRETE  = 1,
    IPMI_SENSOR_TEMPERATURE = 2,
    IPMI_SENSOR_VOLTAGE = 3,
    IPMI_SENSOR_CURRENT = 4,
} IpmiSensorType;

enum { SENSOR_NAME_SIZE = 16 };

#pragma pack(push, 1)
typedef union {
  struct {
      uint32_t type:8; // IpmiSensorType
      uint32_t state:8; // SensorStatus
      uint32_t optional:1;
      uint32_t reserved:15;
  } b;
  uint8_t raw;
} GenericSensorHeader;

typedef struct GenericSensor {
    GenericSensorHeader hdr;
    double value;
    char name[SENSOR_NAME_SIZE];
} GenericSensor;
#pragma pack(pop)

enum { MAX_SENSOR_COUNT = 48 };

typedef struct IpmiSensors {
    uint32_t sensor_count;
    GenericSensor sensors[MAX_SENSOR_COUNT];
} IpmiSensors;

enum { IIC_SENSORS_MAP_START = 0x1000 };
//enum { IIC_SENSORS_MAP_INCR  = 0x40 };
enum { IIC_SENSORS_MAP_SIZE_BYTES = sizeof(GenericSensor) * MAX_SENSOR_COUNT };

void generic_sensor_set_name(GenericSensor *sensor, const char *name);

#endif // IPMI_SENSOR_TYPES_H
