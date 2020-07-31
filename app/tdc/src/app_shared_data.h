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

#ifndef APP_SHARED_DATA_H
#define APP_SHARED_DATA_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISPLAY_SUMMARY,
    DISPLAY_LOG,
    DISPLAY_DIGIPOT,
    DISPLAY_PLL_DETAIL,
    DISPLAY_TASKS,
    DISPLAY_DEVICES,
    DISPLAY_NONE,
} display_mode_t;

extern bool system_power_present;
extern int enable_pll_run;
extern display_mode_t display_mode;
extern int enable_power;
extern int enable_stats_display;

extern int screen_width;
extern int screen_height;

struct Devices;
struct Dev_thset;

struct Devices* getDevices(void);
const struct Devices* getDevicesConst(void);

void schedule_display_refresh(void);
void schedule_display_repaint(void);
bool read_display_refresh(void);
bool read_display_repaint(void);

#ifdef __cplusplus
}
#endif

#endif // APP_SHARED_DATA_H
