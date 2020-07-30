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

#ifndef DEV_SFPIIC_H
#define DEV_SFPIIC_H

#include <stdbool.h>
#include <stdint.h>

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_sfpiic;

void dev_sfpiic_init(struct Dev_sfpiic *d);
DeviceStatus dev_sfpiic_detect(struct Dev_sfpiic *d);
DeviceStatus dev_sfpiic_update(struct Dev_sfpiic *d);
struct Dev_sfpiic_priv;
const struct Dev_sfpiic_priv *get_sfpiic_priv_const(void);
SensorStatus get_sfpiic_sensor_status(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_SFPIIC_H
