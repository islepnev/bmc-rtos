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

#include "dev_digipot_print.h"

#include <stdio.h>

#include "app_shared_data.h"
#include "bsp_digipot.h"
#include "bsp_powermon.h"
#include "commands_digipot.h"
#include "devicebase.h"
#include "devicelist.h"
#include "digipot/dev_digipot_types.h"
#include "display.h"
#include "powermon/dev_powermon_display.h"
#include "powermon/dev_powermon_types.h"

void display_digipots_page(int y, bool repaint)
{
    print_goto(y, 1);
    printf("Voltage adjustments" ANSI_CLEAR_EOL "\n");
    printf("  Keys: UP, DOWN: select channel; +, -: adjust voltage; 0: reset; w: write eeprom" ANSI_CLEAR_EOL "\n");
    print_goto(y+2, 1);
    printf( ANSI_CLEAR_EOL "\n");
    printf("   adjustment ");
    pm_sensor_print_header();
    int cur_y = y+4;
    const DeviceBase *dev_pm = find_device_const(DEV_CLASS_POWERMON);
    const Dev_powermon_priv *pm = dev_pm ? (Dev_powermon_priv *)device_priv_const(dev_pm) : 0;

    const DeviceBase *dev_dp = find_device_const(DEV_CLASS_DIGIPOTS);
    if (dev_dp && dev_dp->priv) {
        const Dev_digipots_priv *dp = (const Dev_digipots_priv *)device_priv_const(dev_dp);

        for (uint i=0; i<dp->count; i++) {
            const Dev_ad5141 *p = &dp->pot[i];
            printf(" %s %s  ", (i == (uint)digipot_screen_selected) ? ">" : " ", potLabel((PotIndex)(i)));
            if (p->dev.device_status == DEVICE_NORMAL)
                printf("%3u ", p->priv.value);
            else
                printf("?   ");
            if (pm && (int)p->priv.sensorIndex > 0 && (int)p->priv.sensorIndex < POWERMON_SENSORS) {
                const pm_sensor *sensor = &pm->sensors.arr[p->priv.sensorIndex];
                const int isOn = monIsOn(pm->sw_state, p->priv.sensorIndex);
                printf("%10s", sensor->priv.label);
                pm_sensor_print_values(sensor, isOn);
            } else {
                printf("<no sensor>");
            }
            printf("%s\n", ANSI_CLEAR_EOL);
        }
        cur_y += dp->count;
    }
    //    pot_debug();
    if (repaint)
        print_clearbox(cur_y, screen_height - 1 - cur_y);
}
