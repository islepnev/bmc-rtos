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

#ifndef DEV_TMP421_H
#define DEV_TMP421_H

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dev_tmp421_priv {
    double temp_internal;
    double temp;
} Dev_tmp421_priv;

typedef struct Dev_tmp421 {
    DeviceBase dev;
    Dev_tmp421_priv priv;
} Dev_tmp421;

bool dev_tmp421_detect(Dev_tmp421 *d);
bool dev_tmp421_read(Dev_tmp421 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_TMP421_H
