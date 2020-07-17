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

#include "dev_ad9545.h"

#include <stdio.h>

#include "ad9545/ad9545_print.h"
#include "ansi_escape_codes.h"

SensorStatus get_pll_sensor_status(const Dev_ad9545 *pll)
{
    if (DEVICE_NORMAL != pll->present)
        return SENSOR_UNKNOWN;
    if ((pll->fsm_state != PLL_STATE_RUN) || (!pll->status.sysclk.b.locked))
        return SENSOR_CRITICAL;
    if (!pll->status.dpll[0].lock_status.b.all_lock)
        return SENSOR_WARNING;
    if (!pll->status.dpll[1].lock_status.b.all_lock)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;

}

const char *dev_ad9545_state_str(ad9545_state_t state)
{
    switch(state) {
    case PLL_STATE_INIT:    return "INIT";
    case PLL_STATE_RESET:    return "RESET";
    case PLL_STATE_SETUP_SYSCLK:    return "SETUP_SYSCLK";
    case PLL_STATE_SYSCLK_WAITLOCK: return ANSI_YELLOW  "SYSCLK_WAITLOCK"     ANSI_CLEAR;
    case PLL_STATE_SETUP:     return ANSI_GREEN  "SETUP"     ANSI_CLEAR;
    case PLL_STATE_RUN:   return ANSI_GREEN    "RUN"   ANSI_CLEAR;
    case PLL_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case PLL_STATE_FATAL:   return ANSI_RED    "FATAL"   ANSI_CLEAR;
    default: return "?";
    }
}

void dev_ad9545_verbose_status(const Dev_ad9545 *d)
{
    printf("AD9545 device FSM state: %s\n", dev_ad9545_state_str(d->fsm_state));
    ad9545_verbose_status(&d->status);
    printf("\n");
}
