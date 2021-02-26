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

#include "bsp_powermon_types.h"

bool pm_switches_isEqual(const pm_switches l, const pm_switches r)
{
    for (int i=0; i<POWER_SWITCH_COUNT; i++)
        if (l[i] != r[i])
            return false;
    return true;
}

const char *psw_label(PowerSwitchIndex index)
{
    switch(index) {
    case PSW_12V:       return "12V";
    case PSW_1V0_CORE:  return "1V0_CORE";
    case PSW_1V0_MGT:   return "1V0_MGT";
    case PSW_1V2_MGT:   return "1V2_MGT";
    case PSW_3V3:       return "3V3";
    case PSW_5V:        return "5V";
    case PSW_ADC_1V8:   return "ADC_1V8";
    case PSW_TDC_2V5:   return "TDC_2V5";
    case PSW_CLOCK_2V5: return "CLK_2V5";
    case PSW_CLOCK_3V3: return "CLK_3V3";
    case PSW_FPGA_1V8:  return "FPGA_1V8";
    }
    return "???";
}

const char *pgood_label(PowerGoodIndex index)
{
    switch(index) {
    case PGOOD_ADC_1V8:   return "1V2_MGT";
    case PGOOD_CLOCK_2V5: return "CLK_2V5";
    case PGOOD_CLOCK_3V3: return "CLK_3V3";
    case PGOOD_FPGA_1V8:  return "FPGA_1V8";
    }
    return "???";
}
