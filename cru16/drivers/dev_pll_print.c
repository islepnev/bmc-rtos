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

#include "dev_pll_print.h"
#include <stdio.h>
#include "ad9545_util.h"

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

static char *pllStateStr(PllState pllState)
{
    switch(pllState) {
    case PLL_STATE_INIT: return "INIT";
    case PLL_STATE_RESET: return "RESET";
    case PLL_STATE_SETUP_SYSCLK: return "SETUP_SYSCLK";
    case PLL_STATE_SYSCLK_WAITLOCK: return "SYSCLK_WAITLOCK";
    case PLL_STATE_SETUP: return "SETUP";
    case PLL_STATE_RUN: return "RUN";
    case PLL_STATE_ERROR: return "ERROR";
    case PLL_STATE_FATAL: return "FATAL";
    default: return "unknown";
    }
}

void pllPrintStatus(const Dev_pll *d)
{
    printf("PLL FSM state %s\n", pllStateStr(d->fsm_state));
    printf("EEPROM status %02X %s%s%s%s\n",
           d->status.eeprom.raw,
           d->status.eeprom.b.load_in_progress ? " LOAD" : "",
           d->status.eeprom.b.save_in_progress ? " SAVE" : "",
           d->status.eeprom.b.fault ? " FAULT" : "",
           d->status.eeprom.b.crc_error ? " CRC error" : ""
           );
    printf("Sysclk status %02X %s%s%s%s%s\n",
           d->status.sysclk.raw,
           d->status.sysclk.b.locked ? " LOCKED" : "",
           d->status.sysclk.b.stable ? " STABLE" : "",
           d->status.sysclk.b.cal_busy ? " CAL_BUSY" : "",
           d->status.sysclk.b.pll0_locked ? " PLL0_LOCK" : "",
           d->status.sysclk.b.pll1_locked ? " PLL1_LOCK" : ""
                                                 );
    printf("misc status %02X %s%s%s\n",
           d->status.misc.raw,
           d->status.misc.b.temp_alarm ? " TEMP_ALARM" : "",
           d->status.misc.b.aux_dpll_lock ? " AUX_DPLL_LOCK" : "",
           d->status.misc.b.aux_dpll_ref_fault ? " AUX_DPLL_REF_FAULT" : ""
                                                 );
    printf("Internal temp %d C\n", d->status.internal_temp/128);
    pllPrintRefStatus(d, REFA);
    pllPrintRefStatus(d, REFB);
    printf(" --- DPLL channel %d ---\n", DPLL0);
    pllPrintDPLLChannelStatus(d, DPLL0);
    printf(" --- DPLL channel %d ---\n", DPLL1);
    pllPrintDPLLChannelStatus(d, DPLL1);
    printf("\n");
}
