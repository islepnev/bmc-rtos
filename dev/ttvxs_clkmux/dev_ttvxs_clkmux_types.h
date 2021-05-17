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

#ifndef DEV_TTVXS_CLKMUX_TYPES_H
#define DEV_TTVXS_CLKMUX_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TTVXS_CLKMUX_STATE_SHUTDOWN,
    TTVXS_CLKMUX_STATE_RESET,
    TTVXS_CLKMUX_STATE_RUN,
    TTVXS_CLKMUX_STATE_PAUSE,
    TTVXS_CLKMUX_STATE_ERROR
} ttvxs_clkmux_state_t;

typedef enum {
    TTVXS_PLL_SOURCE_DIV3 = 0,
    TTVXS_PLL_SOURCE_FMC = 1,
    TTVXS_PLL_SOURCE_EXT = 2,
} ttvxs_pll_source_t;

typedef struct Dev_ttvxs_clkmux_priv {
    uint32_t stateStartTick;
    ttvxs_clkmux_state_t fsm_state;
    ttvxs_pll_source_t pll_source;
} Dev_ttvxs_clkmux_priv;

typedef struct Dev_ttvxs_clkmux {
    DeviceBase dev;
    Dev_ttvxs_clkmux_priv priv;
} Dev_ttvxs_clkmux;

bool ttvxs_clkmux_running(Dev_ttvxs_clkmux *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_TTVXS_CLKMUX_TYPES_H
