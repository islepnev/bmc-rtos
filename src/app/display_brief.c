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

#include <stdio.h>

#include "ad9516/dev_auxpll_print.h"
#include "ad9545/ad9545_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ad9548/dev_ad9548_print.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "devicelist.h"
#include "digipot/dev_digipot.h"
#include "display.h"
#include "display_common.h"
#include "fpga/dev_fpga_print.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_display.h"
#include "sfpiic/dev_sfpiic_print.h"
#include "system_status.h"
#include "thset/dev_thset_print.h"

#if defined(BOARD_TDC64VLE) || defined (BOARD_TDQC)
#include "tqdc_clkmux/dev_tqdc_clkmux_types.h"
#endif
void print_digipots(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_DIGIPOTS);
    if (!d || !d->priv)
        return;
    const Dev_digipots_priv *priv = (const Dev_digipots_priv *)device_priv_const(d);

    printf("POTS: ");
    for (uint i=0; i<priv->count; i++) {
        if (priv->pot[i].dev.device_status == DEVICE_NORMAL)
            printf("%3u ", priv->pot[i].priv.value);
        else
            printf("?   ");
    }
    bool Ok = true;
    for (uint i=0; i<priv->count; i++) {
        if (priv->pot[0].dev.device_status != DEVICE_NORMAL)
        Ok = false;
    }
    if (Ok) {
        printf(Ok ? STR_RESULT_NORMAL : STR_RESULT_WARNING);
    }
    printf("\n");
}

void print_footer(bool repaint)
{
    if (!repaint)
        return;
    print_footer_line();
}

void print_system_status(int y)
{
    print_goto(y, 1);
    const SensorStatus systemStatus = getSystemStatus();
    printf("System status: %s",
           sensor_status_ansi_str(systemStatus));
    printf("\n");

}

void display_pll_detail(int y)
{
#ifdef ENABLE_AD9545
    print_goto(y, 1);
    dev_ad9545_verbose_status();
#endif
#ifdef ENABLE_AD9548
    print_goto(y, 1);
    dev_ad9548_verbose_status();
#endif
}

void display_auxpll_detail(int y)
{
#ifdef ENABLE_AD9516
    print_goto(y, 1);
    printf(" --- AD9516 Status ---\n");
    auxpllPrintStatus();
#endif
}

void print_powermon(int y)
{
#ifdef ENABLE_POWERMON
    print_goto(y, 1);
    print_powermon_box();
#endif
}

void print_pll(int y)
{
    print_goto(y, 1);
#if defined(ENABLE_AD9545)
    dev_ad9545_print_box();
#elif defined(ENABLE_AD9548)
    dev_ad9548_print_box();
#else
#endif
}

void print_auxpll(int y)
{
#ifdef ENABLE_AD9516
    print_goto(y, 1);
    auxpllPrint();
#endif
}

void print_fpga(void)
{
    dev_fpga_print_box();
}

void print_clkmux(void)
{
#if defined(BOARD_TDC64VLE) || defined (BOARD_TDQC)
    const DeviceBase *d = find_device_const(DEV_CLASS_CLKMUX);
    if (!d || !d->priv)
        return;
    const Dev_tqdc_clkmux_priv *priv = (const Dev_tqdc_clkmux_priv *)device_priv_const(d);
    printf("ClkMux: %s %s\n",
           tqdc_clk_source_text(priv->clk_source),
           device_sensor_status_ansi_str(DEV_CLASS_CLKMUX));
#else
    display_device_sensor_ansi_str("ClkMux", DEV_CLASS_CLKMUX);
#endif
}
