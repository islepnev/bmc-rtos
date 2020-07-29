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

#ifndef DEVICELIST_H
#define DEVICELIST_H

#include "dev_common_types.h"
#include "devicebase.h"

enum { DEVICE_LIST_SIZE = 32 };
typedef struct DeviceList {
    int count;
    struct DeviceBase *list[DEVICE_LIST_SIZE];
} DeviceList;

extern DeviceList deviceList;

void create_device(DeviceBase *parent, DeviceBase *d, void *priv, DeviceClass device_class, const BusInterface bus);
DeviceBase *find_device(DeviceClass device_class);
const DeviceBase *find_device_const(DeviceClass device_class);
void *device_priv(DeviceBase *d);
const void *device_priv_const(const DeviceBase *d);

#endif // DEVICELIST_H
