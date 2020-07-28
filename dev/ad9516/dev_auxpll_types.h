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
#ifndef DEV_AUXPLL_TYPES_H
#define DEV_AUXPLL_TYPES_H

#include <stdint.h>
#include "dev_common_types.h"
#include "ad9516/ad9516_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUXPLL_STATE_INIT,
    AUXPLL_STATE_RESET,
    AUXPLL_STATE_SETUP,
    AUXPLL_STATE_RUN,
    AUXPLL_STATE_ERROR,
    AUXPLL_STATE_FATAL
} AuxPllState;

typedef struct Dev_auxpll_priv {
    AD9516_Status status;
    AuxPllState fsm_state;
    uint32_t recoveryCount;
} Dev_auxpll_priv;

typedef struct Dev_auxpll {
    DeviceBase dev;
    Dev_auxpll_priv priv;
} Dev_auxpll;

SensorStatus get_auxpll_sensor_status(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_AUXPLL_TYPES_H
