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

#include "devicelist.h"

#include <assert.h>
#include <string.h>

#include "dev_common_types.h"

DeviceList deviceList = {0};

static void add_child(DeviceBase *list, DeviceBase *d)
{
    if (! list->children) {
        list->children = d;
        return;
    }
    DeviceBase *p = list->children;
    while (p && p->next) {
        p = p->next;
    }
    p->next = d;
}

void create_device(DeviceBase *parent, DeviceBase *d, void *priv, DeviceClass device_class, const BusInterface bus, const char *name)
{
    d->device_class = device_class;
    d->bus = bus;
    strncpy(d->name, name, sizeof(d->name) - 1);
    d->name[sizeof(d->name)-1] = '0';
    d->priv = priv;
    d->parent = parent;
    if (parent) {
        add_child(parent, d);
    }
    assert(deviceList.count < DEVICE_LIST_SIZE);
    if (deviceList.count >= DEVICE_LIST_SIZE) {
        return;
    }
    deviceList.list[deviceList.count++] = d;
}

DeviceBase *find_device(DeviceClass device_class)
{
    for (int i=0; i<deviceList.count; i++) {
        if (deviceList.list[i]->device_class == device_class) {
            return deviceList.list[i];
        }
    }
    return 0;
}

const DeviceBase *find_device_const(DeviceClass class)
{
    return find_device(class);
}

void *device_priv(DeviceBase *d)
{
    assert(d);
    assert(d->priv);
    return d->priv;
}

const void *device_priv_const(const DeviceBase *d)
{
    assert(d);
    assert(d->priv);
    return d->priv;
}
