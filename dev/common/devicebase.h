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

#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <stdbool.h>

#include "dev_common_types.h"
#include "dev_fsm.h"

enum { DEVICE_NAME_LEN = 16 };

typedef struct DeviceBase {
    DeviceClass device_class;
    DeviceStatus device_status;
    SensorStatus sensor;
    BusInterface bus;
    char name[DEVICE_NAME_LEN+1];
    dev_fsm_t fsm;
    void *priv;
    struct DeviceBase *parent;
    struct DeviceBase *children;
    struct DeviceBase *next;
} DeviceBase;

const char *device_class_str(DeviceClass c);
const char *bus_type_str(BusType t);
void set_device_status(DeviceBase *d, const DeviceStatus status);

#endif // DEVICEBASE_H
