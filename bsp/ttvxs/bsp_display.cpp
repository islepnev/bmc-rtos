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

void print_pm_switches(const pm_switches *sw)
{
    printf("Switch 5V main %s   3.3V %s   2.5V %s   1.0V core %s   1.0V mgt %s   1.2V mgt %s   5V FMC %s",
           sw->switch_5v  ? STR_ON : STR_OFF,
           sw->switch_3v3 ? STR_ON : STR_OFF,
           sw->switch_2v5 ? STR_ON : STR_OFF,
           sw->switch_1v0_core ? STR_ON : STR_OFF,
           sw->switch_1v0_mgt ? STR_ON : STR_OFF,
           sw->switch_1v2_mgt ? STR_ON : STR_OFF,
           sw->switch_5v_fmc ? STR_ON : STR_OFF
           );
    printf("%s\n", ANSI_CLEAR_EOL);
}

void pm_pgood_print(const Dev_powermon *pm)
{
    //    printf("Live insert: %s", pm.vmePresent ? STR_RESULT_ON : STR_RESULT_OFF);
    //    printf("%s\n", ANSI_CLEAR_EOL);
    printf("Power good: 3.3V %3s,  2.5V %3s, 1.0V core %3s,  1.0 mgt %3s,  1.2 mgt %3s,  3.3 fmc %3s",
           pm->pgood_3v3      ? STR_ON : STR_OFF,
           pm->pgood_2v5      ? STR_ON : STR_OFF,
           pm->pgood_1v0_core ? STR_ON : STR_OFF,
           pm->pgood_1v0_mgt  ? STR_ON : STR_OFF,
           pm->pgood_1v2_mgt  ? STR_ON : STR_OFF,
           pm->pgood_3v3_fmc  ? STR_ON : STR_OFF
           );
    printf("%s\n", ANSI_CLEAR_EOL);
}
