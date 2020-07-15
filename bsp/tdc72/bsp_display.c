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

#include "bsp_display.h"
#include <stdio.h>
#include "display.h"
#include "dev_powermon_types.h"

void print_pm_switches(const pm_switches sw)
{
    printf("Switch 5V %s   3.3V %s   1.5V %s   1.0V %s   TDC-A %s   TDC-B %s   TDC-C %s",
           sw.switch_5v  ? STR_ON : STR_OFF,
           sw.switch_3v3 ? STR_ON : STR_OFF,
           sw.switch_1v5 ? STR_ON : STR_OFF,
           sw.switch_1v0 ? STR_ON : STR_OFF,
           sw.switch_tdc_a ? STR_ON : STR_OFF,
           sw.switch_tdc_b ? STR_ON : STR_OFF,
           sw.switch_tdc_c ? STR_ON : STR_OFF
           );
    printf("%s\n", ANSI_CLEAR_EOL);
}

void pm_pgood_print(const pm_pgoods pgood)
{
    //    printf("Live insert: %s", pm.vmePresent ? STR_RESULT_ON : STR_RESULT_OFF);
    //    printf("%s\n", ANSI_CLEAR_EOL);
    printf("Power good: 1.5V: %3s, 1.0V core %3s",
           pgood.ltm_pgood ? STR_ON : STR_OFF,
           pgood.fpga_core_pgood ? STR_ON : STR_OFF
           );
    printf("%s\n", ANSI_CLEAR_EOL);
}
