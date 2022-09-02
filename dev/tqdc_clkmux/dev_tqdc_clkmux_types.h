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

#ifndef DEV_TQDC_CLKMUX_TYPES_H
#define DEV_TQDC_CLKMUX_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TQDC_CLK_SOURCE_LOCAL,
    TQDC_CLK_SOURCE_LOCAL_PLL,
    TQDC_CLK_SOURCE_VXS,
    TQDC_CLK_SOURCE_VXS_PLL,
    TQDC_CLK_SOURCE_REFIN,
    TQDC_CLK_SOURCE_REFIN_PLL,
} tqdc_clk_source_t;

inline const char *tqdc_clk_source_text(tqdc_clk_source_t value) {
    switch (value) {
    case TQDC_CLK_SOURCE_LOCAL: return "Local";
    case TQDC_CLK_SOURCE_LOCAL_PLL: return "Local PLL";
    case TQDC_CLK_SOURCE_VXS: return "VXS";
    case TQDC_CLK_SOURCE_VXS_PLL: return "VXS PLL";
    case TQDC_CLK_SOURCE_REFIN: return "REF";
    case TQDC_CLK_SOURCE_REFIN_PLL: return "REF PLL";
    default: return "";
    }
}
typedef struct Dev_tqdc_clkmux_priv {
    tqdc_clk_source_t clk_source;
} Dev_tqdc_clkmux_priv;

typedef struct Dev_tqdc_clkmux {
    DeviceBase dev;
    Dev_tqdc_clkmux_priv priv;
} Dev_tqdc_clkmux;

bool tqdc_clkmux_running(Dev_tqdc_clkmux *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_TQDC_CLKMUX_TYPES_H
