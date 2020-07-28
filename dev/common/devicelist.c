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

void create_device(DeviceBase *parent, DeviceBase *d, void *priv, DeviceClass class, const BusInterface bus)
{
    d->class = class;
    d->bus = bus;
    d->priv = priv;
    d->parent = parent;
    if (parent) {
        add_child(parent, d);
    }
    if (deviceList.count >= DEVICE_LIST_SIZE) {
        return;
    }
    deviceList.list[deviceList.count++] = d;
}

DeviceBase *find_device(DeviceClass class)
{
    for (int i=0; i<deviceList.count; i++) {
        if (deviceList.list[i]->class == class) {
            assert(deviceList.list[i]);
            return deviceList.list[i];
        }
    }
    assert(0);
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
