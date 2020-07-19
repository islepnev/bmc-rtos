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
//#include "ad9516_util.h"
/*
void pllPrintRefStatus(const Dev_pll *d, PllRef_TypeDef ref_input)
{
    Ref_Status_REG_Type r = d->status.ref[ref_input];
    printf("Ref %d  %02X",
           ref_input,
           r.raw
           );
    pllPrintRefStatusBits(r);
    printf("\n");
}

void pllPrintDPLLChannelStatus(const Dev_pll *d, PllChannel_TypeDef channel)
{
    const DPLL_Status *dpll_status = &d->status.dpll[channel];
    {
        DPLL_Active_Profile_REG_Type r = dpll_status->act_profile;
        printf("Translation profile %d.%s%s%s%s%s%s%s\n",
               channel,
               ((r.raw & 0x3F) ==0) ? "none" : "",
               r.b.profile_0 ? "0" : "",
               r.b.profile_1 ? "1" : "",
               r.b.profile_2 ? "2" : "",
               r.b.profile_3 ? "3" : "",
               r.b.profile_4 ? "4" : "",
               r.b.profile_5 ? "5" : ""
                               );
    }
    {
        DPLL_Lock_Status_REG_Type r = dpll_status->lock_status;
        printf("Lock status %02X %s%s%s%s%s%s\n",
               r.raw,
               r.b.all_lock ? " ALL_LOCK" : "",
               r.b.dpll_phase_lock ? " D_PHASE_LOCK" : "",
               r.b.dpll_freq_lock ? " D_FREQ_LOCK" : "",
               r.b.apll_lock ? " A_LOCK" : "",
               r.b.apll_cal_busy ? " A_CAL_BUSY" : "",
               r.b.apll_cal_done ? " A_CALIBRATED" : ""
                                   );
    }
    {
        DPLL_Operation_REG_Type r = dpll_status->operation;
        printf("Oper status %02X %s%s%s%s %s %d.%d\n",
               r.raw,
               r.b.freerun ? " FREERUN" : "",
               r.b.holdover ? " HOLDOVER" : "",
               r.b.ref_switch ? " REF_SWITCH" : "",
               r.b.active ? " ACTIVE" : "",
               r.b.active ? "current profile" : "last profile",
               channel,
               r.b.active_profile
               );
    }
    {
        DPLL_State_REG_Type r = dpll_status->state;
        printf("State %02X %s%s%s%s%s\n",
               r.raw,
               r.b.hist_available   ? " HIST" : "",
               r.b.freq_clamp       ? " FREQ_CLAMP" : "",
               r.b.phase_slew_limit ? " PHASE_SLEW_LIMIT" : "",
               r.b.facq_active      ? " FACQ_ACT" : "",
               r.b.facq_done        ? " FACQ_DONE" : ""
                                      );
    }
    {
        uint64_t ftw = dpll_status->ftw_history;
        double ppb = pll_ftw_rel_ppb(d, channel);
        if (dpll_status->state.b.freq_clamp)
            printf("TW history %lld (FREQ_CLAMP)\n", ftw);
        else
            printf("TW history %lld (%lld ppb)\n", ftw, (int64_t) ppb);
    }

    printf("PLD tub %d, FLD tub %d\n", dpll_status->pld_tub, dpll_status->fld_tub);

    printf("Phase slew %02X %s%s%s%s\n",
           dpll_status->phase_slew,
           (dpll_status->phase_slew & 0x1) ? " Q1A" : "",
           (dpll_status->phase_slew & 0x2) ? " Q1AA" : "",
           (dpll_status->phase_slew & 0x4) ? " Q1B" : "",
           (dpll_status->phase_slew & 0x8) ? " Q1BB" : ""
                                             );
    printf("Phase control error %02X %s%s%s%s\n",
           dpll_status->phase_control_error,
           (dpll_status->phase_control_error & 0x1) ? " Q1A" : "",
           (dpll_status->phase_control_error & 0x2) ? " Q1AA" : "",
           (dpll_status->phase_control_error & 0x4) ? " Q1B" : "",
           (dpll_status->phase_control_error & 0x8) ? " Q1BB" : ""
                                                      );
}
*/
static char *auxpllStateStr(AuxPllState auxpllState)
{
    switch(auxpllState) {
    case AUXPLL_STATE_INIT: return "INIT";
    case AUXPLL_STATE_RESET: return "RESET";
    case AUXPLL_STATE_SETUP_SYSCLK: return "SETUP_SYSCLK";
    case AUXPLL_STATE_SYSCLK_WAITLOCK: return "SYSCLK_WAITLOCK";
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
