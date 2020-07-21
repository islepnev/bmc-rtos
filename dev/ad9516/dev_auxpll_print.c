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

#include "dev_auxpll_print.h"
#include <stdio.h>
#include "dev_auxpll_types.h"
#include "ansi_escape_codes.h"

static char *auxpllStateStr(AuxPllState auxpllState)
{
    switch(auxpllState) {
    case AUXPLL_STATE_INIT: return "INIT";
    case AUXPLL_STATE_RESET: return "RESET";
    case AUXPLL_STATE_SETUP: return "SETUP";
    case AUXPLL_STATE_RUN: return "RUN";
    case AUXPLL_STATE_ERROR: return "ERROR";
    case AUXPLL_STATE_FATAL: return "FATAL";
    default: return "unknown";
    }
}

void auxpllPrintStatus(const Dev_auxpll *d)
{
    // printf("AUXPLL FSM state %s\n", auxpllStateStr(d->fsm_state));
    printf("PLL readback %02X %s%s%s%s%s%s\n",
           d->status.pll_readback.raw,
           d->status.pll_readback.b.dlock ? " DLOCK" : "",
           d->status.pll_readback.b.ref1_over_thr ? " REF1_over" : "",
           d->status.pll_readback.b.ref2_over_thr ? " REF2_over" : "",
           d->status.pll_readback.b.vco_over_thr ? " VCO_over" : "",
           d->status.pll_readback.b.ref2_selected ? " REF2_SEL" : "",
           d->status.pll_readback.b.vco_cal_finished ? " VCO_CAL_OK" : ""
           );
    printf("\n");
}

void auxpllPrint(const Dev_auxpll *d)
{
    printf("PLL AD9516:   %s %s",
           auxpllStateStr(d->fsm_state),
           sensor_status_ansi_str(get_auxpll_sensor_status(d)));
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("  ");
    auxpllPrintStatus(d);
}
