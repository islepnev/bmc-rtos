/*
**    Copyright 2019 Ilja Slepnev
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

#ifndef DEV_TTVXS_CLKMUX_H
#define DEV_TTVXS_CLKMUX_H

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_ttvxs_clkmux;

void dev_ttvxs_clkmux_init(struct Dev_ttvxs_clkmux *d);
DeviceStatus dev_ttvxs_clkmux_detect(struct Dev_ttvxs_clkmux *d);
DeviceStatus dev_ttvxs_clkmux_set(struct Dev_ttvxs_clkmux *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_TTVXS_CLKMUX_H
