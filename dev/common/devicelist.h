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

typedef struct DeviceListEl {
    struct DeviceBase *dev;
    struct DeviceListEl *next;
} DeviceListEl;

enum { DEVICE_LIST_SIZE = 16 };
typedef struct DeviceList {
    int count;
    struct DeviceBase *list[DEVICE_LIST_SIZE];
} DeviceList;

extern DeviceList deviceList;

#endif // DEVICELIST_H
