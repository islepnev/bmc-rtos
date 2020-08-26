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

#include "dev_ad9548_print.h"

#include <assert.h>
#include <stdio.h>

#include "ad9548/ad9548_print.h"
#include "ansi_escape_codes.h"
#include "dev_ad9548.h"
#include "display.h"
#include "devicelist.h"

static const char *dev_ad9548_state_str(ad9548_state_t state)
{
    switch(state) {
    case AD9548_STATE_INIT:    return "INIT";
    case AD9548_STATE_RESET:    return "RESET";
    case AD9548_STATE_SETUP_SYSCLK:    return "SETUP_SYSCLK";
    case AD9548_STATE_SYSCLK_WAITLOCK: return ANSI_YELLOW  "SYSCLK_WAITLOCK"     ANSI_CLEAR;
    case AD9548_STATE_SETUP:     return ANSI_GREEN  "SETUP"     ANSI_CLEAR;
    case AD9548_STATE_RUN:   return ANSI_GREEN    "RUN"   ANSI_CLEAR;
    case AD9548_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case AD9548_STATE_FATAL:   return ANSI_RED    "FATAL"   ANSI_CLEAR;
    default: return "?";
    }
}

void dev_ad9548_verbose_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9548);
    if (!d || !d->priv)
        return;
    const Dev_ad9548_priv *priv = (const Dev_ad9548_priv *)device_priv_const(d);
    printf(" --- AD9548 Setup ---\n");
//    ad9548_verbose_setup(&priv->setup);
    printf(" --- AD9548 Status ---\n");
//    printf("AD9548 device FSM state: %s\n", dev_ad9548_state_str(priv->fsm_state));
    ad9548_verbose_status(&priv->status);
    printf("\n");
}

void dev_ad9548_print_box(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9548);
    if (!d || !d->priv)
        return;
    const Dev_ad9548_priv *priv = (const Dev_ad9548_priv *)device_priv_const(d);
    printf("PLL AD9548:      %s %s",
           dev_ad9548_state_str(priv->fsm_state),
           sensor_status_ansi_str(d->sensor));
    print_clear_eol();
    ad9548_brief_status(&priv->status);
}
