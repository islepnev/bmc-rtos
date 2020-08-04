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

#ifndef DEV_THSET_TYPES_H
#define DEV_THSET_TYPES_H

#include <stdint.h>

#include "dev_common_types.h"
#include "devicebase.h"
#include "ipmi_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    THSET_STATE_0,
    THSET_STATE_1,
    THSET_STATE_2,
} thset_state_t;

enum {DEV_THSET_MAX_COUNT = 8};

typedef struct Dev_thset_priv {
    thset_state_t state;
    int count;
    GenericSensor sensors[DEV_THSET_MAX_COUNT];
} Dev_thset_priv;

typedef struct Dev_thset {
    DeviceBase dev;
    Dev_thset_priv priv;
} Dev_thset;

const Dev_thset_priv *get_thset_priv_const(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_THSET_TYPES_H
