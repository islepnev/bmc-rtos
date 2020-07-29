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

#ifndef DEV_ADT7301_H
#define DEV_ADT7301_H

#include <stdbool.h>
#include <stdint.h>

#include "dev_common_types.h"
#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADT7301_STATE_SHUTDOWN,
    ADT7301_STATE_RESET,
    ADT7301_STATE_RUN,
    ADT7301_STATE_PAUSE,
    ADT7301_STATE_ERROR
} dev_adt7301_state_t;

typedef struct Dev_adt7301_priv {
    dev_adt7301_state_t state;
    uint32_t state_start_tick;
    double temp;
} Dev_adt7301_priv;

typedef struct Dev_adt7301 {
    DeviceBase dev;
    Dev_adt7301_priv priv;
} Dev_adt7301;

bool dev_adt7301_detect(Dev_adt7301 *d);
bool dev_adt7301_read(Dev_adt7301 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_ADT7301_H
