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

#include "app_shared_data.h"

#include "devices_types.h"

static Devices dev = {};
bool system_power_present = false;
int enable_pll_run = 0;
display_mode_t display_mode = DISPLAY_SUMMARY;
int enable_power = 1;
int enable_stats_display = 1;

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

Dev_powermon* get_dev_powermon(void)
{
    return &dev.pm;
}

const Dev_powermon* get_dev_powermon_const(void)
{
    return &dev.pm;
}

Dev_fpga* get_dev_fpga(void)
{
    return &dev.fpga;
}

Dev_ttvxs_clkmux* get_dev_clkmux(void)
{
    return &dev.clkmux;
}

Dev_ad9545* get_dev_pll(void)
{
    return &dev.pll;
}

Dev_auxpll* get_dev_auxpll(void)
{
    return &dev.auxpll;
}

Dev_thset *get_dev_thset(void)
{
    return &dev.thset;
}

Dev_sfpiic *get_dev_sfpiic(void)
{
    return &dev.sfpiic;
}

Dev_eeprom_config *get_dev_eeprom_config(void)
{
    return &dev.eeprom_config;
}
static int display_refresh_flag = 0;

void schedule_display_refresh(void)
{
    display_refresh_flag = 1;
}

int read_display_refresh(void)
{
    // FIXME: make atomic
    int value = display_refresh_flag;
    display_refresh_flag = 0;
    return value;
}
