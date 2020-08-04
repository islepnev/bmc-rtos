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

#ifndef DEV_DIGIPOT_H
#define DEV_DIGIPOT_H

#include <stdint.h>
#include "bus/bus_types.h"
#include "dev_digipot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int digipot_detect(Dev_digipots *d);
void digipot_read_rdac_all(Dev_digipots *d);
void dev_ad5141_reset(Dev_ad5141 *d);
void dev_ad5141_inc(Dev_ad5141 *d);
void dev_ad5141_dec(Dev_ad5141 *d);
void dev_ad5141_write(Dev_ad5141 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_DIGIPOT_H
