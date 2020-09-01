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

#ifndef DEV_CRU16_CLKMUX_TYPES_H
#define DEV_CRU16_CLKMUX_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CRU16_CLKMUX_STATE_RESET,
    CRU16_CLKMUX_STATE_RUN,
    CRU16_CLKMUX_STATE_PAUSE,
    CRU16_CLKMUX_STATE_ERROR
} cru16_clkmux_state_t;

typedef enum {
    CRU16_PLL_SOURCE_DIV3 = 0,
    CRU16_PLL_SOURCE_FMC = 1,
    CRU16_PLL_SOURCE_EXT = 2,
} cru16_pll_source_t;

typedef struct Dev_cru16_clkmux_priv {
    uint32_t stateStartTick;
    cru16_clkmux_state_t fsm_state;
    cru16_pll_source_t pll_source;
} Dev_cru16_clkmux_priv;

typedef struct Dev_cru16_clkmux {
    DeviceBase dev;
    Dev_cru16_clkmux_priv priv;
} Dev_cru16_clkmux;

bool cru16_clkmux_running(const Dev_cru16_clkmux *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_CRU16_CLKMUX_TYPES_H
