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

// dummy board specific functions

#include "bsp_powermon.h"

int monIsOn(const pm_switches sw, SensorIndex index)
{
    (void)sw;
    (void)index;
    return 0;
}

bool readLiveInsertPin(void)
{
    return 1;
}

void init_power_switches(pm_switches sw)
{
    for (int i=0; i<POWER_SWITCH_COUNT; i++)
        sw[i] = true;
}

void read_power_switches_state(pm_switches sw_state)
{
}

void write_power_switches(pm_switches sw)
{
}

void init_pgood(pm_pgoods pgood)
{
}

static bool readPowerGoodFpga(void)
{
    return 1;
}

static bool readPowerGood1v5(void)
{
    return 1;
}

void pm_read_pgood(pm_pgoods pgood)
{
}

bool get_all_pgood(const pm_pgoods pgood)
{
    for (int i=0; i<POWER_GOOD_COUNT; i++)
        if (!pgood[i])
            return false;
    return true;
}

bool get_input_power_valid(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return true;
}

bool get_input_power_normal(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return true;
}

bool get_input_power_failed(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return false;
}

double pm_get_power_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

double pm_get_power_max_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

double pm_get_fpga_power_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

void switch_power(Dev_powermon_priv *p, bool state)
{
}
