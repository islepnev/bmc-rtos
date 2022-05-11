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
#include <stdio.h>

#include "ansi_escape_codes.h"

const char *short_device_status_str(DeviceStatus status)
{
    switch (status) {
    case DEVICE_UNKNOWN:
        return "   ?";
    case DEVICE_NORMAL:
        return "  Ok";
    case DEVICE_FAIL:
        return ANSI_RED "FAIL" ANSI_CLEAR;
    default:
        return "   ?";
    }
}

void devicelist_print(DeviceBase *d, int depth)
{
    if (!d) {
        return;
    }
    const BusInterface *bus = &d->bus;
    enum {BUFSZ = 16};
    char str[BUFSZ+1] = {0};
    int len = 0;
    for (int i=0; i<depth && len < BUFSZ; i++) {
        str[len++] = ' ';
        str[len++] = ' ';
    }
    printf("%s ", str);
    if (d->bus.type == BUS_NONE)
        printf("--------");
    else
        printf("%s %d.%02X",
               bus_type_str(bus->type),
               bus->bus_number,
               bus->address
               );
    printf("    %s '%s'" ANSI_COL50 "%s",
           device_class_str(d->device_class),
           d->name,
           short_device_status_str(d->device_status));
    printf("\n");

    DeviceBase *p = d->children;
    while (p) {
        devicelist_print(p, depth + 1);
        p = p->next;
    }
}
