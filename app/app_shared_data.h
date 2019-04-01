//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef APP_SHARED_DATA_H
#define APP_SHARED_DATA_H

#include "devices.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISPLAY_SUMMARY,
    DISPLAY_PLL_DETAIL,
    DISPLAY_NONE,
} display_mode_t;

extern Devices dev;
extern int enable_pll_run;
extern display_mode_t display_mode;
extern int enable_power;
extern int enable_stats_display;

const Devices* getDevices(void);

#ifdef __cplusplus
}
#endif

#endif // APP_SHARED_DATA_H
