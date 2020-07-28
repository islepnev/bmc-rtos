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

typedef enum {
    DEV_CLASS_0, // no class
    DEV_CLASS_VIRTUAL, // no bus
    DEV_CLASS_AUXPLL,
    DEV_CLASS_DIGIPOT,
    DEV_CLASS_FPGA,
    DEV_CLASS_PLL,
    DEV_CLASS_POWERMON,
    DEV_CLASS_SFP,
    DEV_CLASS_THERM,
    DEV_CLASS_VXSIICM,
    DEV_CLASS_VXSIICS,
} DeviceClass;

typedef struct DeviceBase {
    DeviceClass class;
    DeviceStatus device_status; // old name: 'present'
    SensorStatus sensor;
    BusInterface bus;
    void *priv;
} DeviceBase;

enum { DEVICE_LIST_SIZE = 16 };
typedef struct DeviceList {
    int count;
    DeviceBase *list[DEVICE_LIST_SIZE];
} DeviceList;

const char *sensor_status_ansi_str(SensorStatus state);
const char *sensor_status_text(SensorStatus state);
char *deviceStatusResultStr(DeviceStatus status);

extern DeviceList deviceList;
void create_device(DeviceBase *d, void *priv, DeviceClass class, const BusInterface bus);
DeviceBase *find_device(DeviceClass class);
const DeviceBase *find_device_const(DeviceClass class);
void *device_priv(DeviceBase *d);
const void *device_priv_const(const DeviceBase *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_COMMON_TYPES_H
