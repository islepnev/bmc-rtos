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

static Devices dev = {0};
int enable_pll_run = 0;
display_mode_t display_mode = DISPLAY_BOARDS; // FIXME DISPLAY_SUMMARY;
int enable_power = 1;
int enable_stats_display = 1;

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

Dev_pll* get_dev_pll(void)
{
    return &dev.pll;
}

Dev_thset *get_dev_thset(void)
{
    return &dev.thset;
}

Dev_vxsiic *get_dev_vxsiic(void)
{
    return &dev.vxsiic;
}

Dev_sfpiic *get_dev_sfpiic(void)
{
    return &dev.sfpiic;
}

Dev_at24c *get_dev_eeprom_config(void)
{
    return &dev.eeprom_config;
}
