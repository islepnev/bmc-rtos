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

#include "dev_common_types.h"

typedef struct DeviceBase {
    DeviceClass device_class;
    DeviceStatus device_status;
    SensorStatus sensor;
    BusInterface bus;
    void *priv;
    struct DeviceBase *parent;
    struct DeviceBase *children;
    struct DeviceBase *next;
} DeviceBase;


#endif // DEVICEBASE_H
