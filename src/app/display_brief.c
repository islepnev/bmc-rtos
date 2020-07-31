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

#include "display_brief.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ad9516/dev_auxpll_print.h"
#include "ad9545/ad9545_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp_powermon.h"
#include "cmsis_os.h"
#include "clkmux/dev_clkmux.h"
#include "debug_helpers.h"
#include "dev_common_types.h"
#include "dev_mcu.h"
#include "devices_types.h"
#include "digipot/dev_digipot.h"
#include "digipot/dev_digipot_print.h"
#include "display.h"
#include "display_boards.h"
#include "display_brief.h"
#include "display_common.h"
#include "display_log.h"
#include "display_tasks.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "fpga/dev_fpga_print.h"
#include "fpga/dev_fpga_types.h"
#include "logbuffer.h"
#include "logentry.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_display.h"
#include "powermon/dev_powermon_types.h"
#include "sfpiic/dev_sfpiic_print.h"
#include "stm32f7xx_hal.h"
#include "system_status.h"
#include "thset/dev_thset_print.h"

void print_pm_pots(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_DIGIPOTS);
    if (!d || !d->priv)
        return;
    const Dev_digipots_priv *priv = (const Dev_digipots_priv *)device_priv_const(d);

    printf("POTS: ");
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        if (priv->pot[i].dev.device_status == DEVICE_NORMAL)
            printf("%3u ", priv->pot[i].priv.value);
        else
            printf("?   ");
    }
    bool Ok = true;
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        if (priv->pot[0].dev.device_status != DEVICE_NORMAL)
        Ok = false;
    }
    if (Ok) {
        printf(Ok ? STR_RESULT_NORMAL : STR_RESULT_WARNING);
    }
    printf("%s\n", ANSI_CLEAR_EOL);
}

void print_footer(void)
{
    print_goto(screen_height, 1);
    print_footer_line();
}

void print_system_status(int y)
{
    print_goto(y, 1);
    const SensorStatus systemStatus = getSystemStatus();
    printf("System status: %s",
           sensor_status_ansi_str(systemStatus));
    print_clear_eol();

}

void display_pll_detail(int y)
{
    print_clearbox(y, DISPLAY_PLL_DETAIL_H);
    print_goto(y, 1);
    dev_ad9545_verbose_status();
}

void display_auxpll_detail(int y)
{
    print_clearbox(y, DISPLAY_AUXPLL_DETAIL_H);
    print_goto(y, 1);
    printf(" --- AD9516 Status ---\n");
    auxpllPrintStatus();
}

void print_powermon(int y)
{
    print_goto(y, 1);
    print_powermon_box();
}

void print_pll(int y)
{
    print_goto(y, 1);
    dev_ad9545_print_box();
}

void print_auxpll(int y)
{
    print_goto(y, 1);
    auxpllPrint();
}

void print_thset(int y)
{
    print_goto(y, 1);
    print_thset_box();
}

void print_fpga(int y)
{
    print_goto(y, 1);
    dev_fpga_print_box();
}

void print_clkmux(int y)
{
    print_goto(y, 1);
    display_device_sensor_ansi_str("ClkMux", DEV_CLASS_CLKMUX);
}

