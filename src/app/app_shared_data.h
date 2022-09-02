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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISPLAY_SUMMARY,
    DISPLAY_LOG,
    DISPLAY_BOARDS,
    DISPLAY_DIGIPOT,
    DISPLAY_PLL_DETAIL,
    DISPLAY_SFP_DETAIL,
    DISPLAY_TASKS,
    DISPLAY_DEVICES,
//    DISPLAY_MENU,
    DISPLAY_MODE_COUNT // last element
} display_mode_t;

extern bool system_power_present;
//extern int enable_pll_run;
extern display_mode_t display_mode;
extern int enable_power;
extern bool eth_link_up;
extern int app_ipv4;
extern const char *eth_port_descr;
extern int screen_width;
extern int screen_height;
extern bool screen_size_set;

extern bool main_clock_ready;
extern bool aux_clock_ready;

typedef enum {
    CLOCK_SOURCE_AUTO = 0,
    CLOCK_SOURCE_LOCAL = 1,
    CLOCK_SOURCE_TTC = 2,
    CLOCK_SOURCE_VXS = 4
} clock_source_t;

typedef struct clock_control_t {
    bool pll_bypass;
    clock_source_t valid;
    clock_source_t source;
} clock_control_t;
extern clock_control_t clock_control;

typedef struct pll_clock_shift_command_t {
    int req;
    int ack;
    int numer;
    int denom;
} pll_clock_shift_command_t;

extern pll_clock_shift_command_t pll_clock_shift_command;

void schedule_display_refresh(void);
void schedule_display_repaint(void);
void schedule_display_reset(void);
bool read_display_refresh(void);
bool read_display_repaint(void);
bool read_display_reset(void);

#ifdef __cplusplus
}
#endif

#endif // APP_SHARED_DATA_H
