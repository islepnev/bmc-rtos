/*
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef DEV_COMMON_TYPES_H
#define DEV_COMMON_TYPES_H

#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SENSOR_UNKNOWN  = 0,
    SENSOR_NORMAL   = 1, // numeric value used
    SENSOR_WARNING  = 2,
    SENSOR_CRITICAL = 3
} SensorStatus;

typedef enum {
    DEVICE_UNKNOWN = 0,
    DEVICE_NORMAL = 1,
    DEVICE_FAIL = 2
} DeviceStatus;

typedef enum {
    DEV_OK       = 0,
    IIC_ERROR    = 1,
    IIC_BUSY     = 2,
    IIC_TIMEOUT  = 3,
    DEV_ERROR    = 0x20U,
} OpStatusTypeDef;

typedef struct DeviceBase {
    DeviceStatus device_status; // old name: 'present'
    BusInterface bus;
    void *priv;
} DeviceBase;

const char *sensor_status_ansi_str(SensorStatus state);
const char *sensor_status_text(SensorStatus state);
char *deviceStatusResultStr(DeviceStatus status);

#ifdef __cplusplus
}
#endif

#endif // DEV_COMMON_TYPES_H
