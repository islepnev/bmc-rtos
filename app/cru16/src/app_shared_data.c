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

#include "app_shared_data.h"

#include "devices_types.h"

static Devices dev = {};
bool system_power_present = false;
int enable_pll_run = 0;
display_mode_t display_mode = DISPLAY_SUMMARY;
int enable_power = 1;
int enable_stats_display = 1;

int screen_width = 100;
int screen_height = 35;

int get_mcb_pcb_ver(void)
{
    Devices* d = getDevices();
    return d->pcb_ver;
}

Devices* getDevices(void)
{
    return &dev;
}

const Devices* getDevicesConst(void)
{
    return &dev;
}

static bool display_refresh_flag = 0;
static bool display_repaint_flag = 1;

void schedule_display_refresh(void)
{
    display_refresh_flag = 1;
}

void schedule_display_repaint(void)
{
    display_repaint_flag = 1;
}

bool read_display_refresh(void)
{
    // FIXME: make atomic
    int value = display_refresh_flag;
    display_refresh_flag = 0;
    return value;
}

bool read_display_repaint(void)
{
    // FIXME: make atomic
    int value = display_repaint_flag;
    display_repaint_flag = 0;
    return value;
}
