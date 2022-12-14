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

#ifndef DEV_DIGIPOT_TYPES_H
#define DEV_DIGIPOT_TYPES_H

#include <stdint.h>
#include "bus/bus_types.h"
#include "dev_common_types.h"
#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dev_ad5141_priv {
    //    PotIndex index;
    //    SensorIndex sensorIndex;
    int index;
    int sensorIndex;
    uint8_t value;
} Dev_ad5141_priv;

typedef struct Dev_ad5141 {
    DeviceBase dev;
    Dev_ad5141_priv priv;
} Dev_ad5141;

typedef enum {
    DIGIPOT_STATE_INIT,
    DIGIPOT_STATE_DETECT,
    DIGIPOT_STATE_RUN,
    DIGIPOT_STATE_ERROR,
    DIGIPOT_STATE_FATAL
} digipot_state_t;

enum {MAX_DIGIPOT_COUNT = 4};

typedef struct Dev_digipots_priv {
    unsigned int count;
    uint32_t stateStartTick;
    digipot_state_t state;
    Dev_ad5141 pot[MAX_DIGIPOT_COUNT];
} Dev_digipots_priv;

typedef struct Dev_digipots {
    DeviceBase dev;
    Dev_digipots_priv priv;
} Dev_digipots;

SensorStatus get_digipot_sensor_status(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_DIGIPOT_TYPES_H
