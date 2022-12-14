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

#include "ad9545_print.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "ad9545_setup.h"
#include "ad9545_setup_regs.h"
#include "display.h"

static int64_t pll_ftw_rel_ppb(const AD9545_Status *status, AD9545_Channel_TypeDef channel)
{
    const AD9545_DPLL_Status *dpll_status = &status->dpll[channel];
    uint64_t ftw = dpll_status->ftw_history;
    uint64_t default_ftw = get_dpll_default_ftw(channel);
    int64_t twdelta = ftw - default_ftw;
    int64_t norm = default_ftw/1000000000ULL;
    return twdelta/norm;
}

static AD9545_Profile_Ref_Source_TypeDef pll_get_current_ref(const AD9545_Status *status, AD9545_Channel_TypeDef channel)
{
    //    int active = d->status.dpll[channel].operation.b.active;
    //    if (!active)
    //        return PROFILE_REF_SOURCE_INVALID;
    uint8_t active_profile = status->dpll[channel].operation.b.active_profile;
    if (active_profile >= AD9545_DPLL_PROFILE_COUNT)
        return PROFILE_REF_SOURCE_INVALID;
    AD9545_DPLL_Setup_TypeDef dpll = {0};
    init_DPLL_Setup(&dpll, channel);
    return (AD9545_Profile_Ref_Source_TypeDef)dpll.profile[active_profile].Profile_Ref_Source;
}

static void pllPrintRefStatusBits(AD9545_Ref_Status_REG_Type r)
{
    printf("%s%s%s%s%s%s",
           r.b.slow   ? " SLOW" : "",
           r.b.fast   ? " FAST" : "",
           r.b.jitter ? " JITTER" : "",
           r.b.fault  ? " FAULT" : "",
           r.b.valid  ? " VALID" : "",
           r.b.los    ? " LOS" : ""
           );
}

static const char *pllProfileRefSourceStr(AD9545_Profile_Ref_Source_TypeDef r)
{
    switch(r) {
    case PROFILE_REF_SOURCE_A: return "A";
    case PROFILE_REF_SOURCE_AA: return "AA";
    case PROFILE_REF_SOURCE_B: return "B";
    case PROFILE_REF_SOURCE_BB: return "BB";
    case PROFILE_REF_SOURCE_DPLL0: return "DPLL0";
    case PROFILE_REF_SOURCE_DPLL1: return "DPLL1";
    case PROFILE_REF_SOURCE_NCO0: return "NCO0";
    case PROFILE_REF_SOURCE_NCO1: return "NCO1";
    case PROFILE_REF_SOURCE_INVALID: return "";
    default: return "";
    }
}

static void pllPrintRefStatus(const AD9545_Status *status, AD9545_Ref_TypeDef ref_input)
{
    AD9545_Ref_Status_REG_Type r = status->ref[ref_input];
    printf("Ref %d  %02X",
           ref_input,
           r.raw
           );
    pllPrintRefStatusBits(r);
    printf("\n");
}

static void pllPrintDPLLChannelStatus(const AD9545_Status *status, AD9545_Channel_TypeDef channel)
{
    const AD9545_DPLL_Status *dpll_status = &status->dpll[channel];
    {
        AD9545_DPLL_Active_Profile_REG_Type r = dpll_status->act_profile;
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
        AD9545_DPLL_Lock_Status_REG_Type r = dpll_status->lock_status;
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
        AD9545_DPLL_Operation_REG_Type r = dpll_status->operation;
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
        AD9545_DPLL_State_REG_Type r = dpll_status->state;
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
        double ppb = pll_ftw_rel_ppb(status, channel);
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

//static char *ad9545_state_text(ad9545_state_t pllState)
//{
//    switch(pllState) {
//    case PLL_STATE_INIT: return "INIT";
//    case PLL_STATE_RESET: return "RESET";
//    case PLL_STATE_SETUP_SYSCLK: return "SETUP_SYSCLK";
//    case PLL_STATE_SYSCLK_WAITLOCK: return "SYSCLK_WAITLOCK";
//    case PLL_STATE_SETUP: return "SETUP";
//    case PLL_STATE_RUN: return "RUN";
//    case PLL_STATE_ERROR: return "ERROR";
//    case PLL_STATE_FATAL: return "FATAL";
//    default: return "unknown";
//    }
//}
static void pllPrintDPLLChannelSetup(const AD9545_DPLL_Setup_TypeDef *dpll, AD9545_Channel_TypeDef channel)
{

    printf("DPLL%d profile[0] %s%s%s%s, FB %s%s FBdiv %lu+%lu/%lu, BW %g Hz\n",
           channel,
           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_A) ? "RefA" : "",
           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_B) ? "RefB" : "",
           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_DPLL0) ? "DPLL0" : "",
           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_DPLL1) ? "DPLL1" : "",
           (dpll->profile[0].ZD_Feedback_Path == PROFILE_EXT_ZD_FEEDBACK_REFA) ? "RefA" : "",
           (dpll->profile[0].ZD_Feedback_Path == PROFILE_EXT_ZD_FEEDBACK_REFB) ? "RefB" : "",
           dpll->profile[0].Buildout_FB_Divider,
           dpll->profile[0].Buildout_FB_Fraction,
           dpll->profile[0].Buildout_FB_Modulus,
           dpll->profile[0].Loop_BW / 1e6
           );
}

void ad9545_verbose_setup(const ad9545_setup_t *setup)
{
    /*
    Pll_OutputDrivers_Setup_TypeDef out_drivers;
    Pll_OutputDividers_Setup_TypeDef out_dividers;
    Pll_DPLLMode_Setup_TypeDef dpll_mode;
    PllSysclkSetup_TypeDef sysclk;
*/
    printf("RefA: % 3g ns, Rdiv %lu\n",
           setup->ref.REFA_Input_Period * 1e-9,
           setup->ref.REFA_R_Divider
           );
    printf("RefB: % 3g ns, Rdiv %lu\n",
           setup->ref.REFB_Input_Period * 1e-9,
           setup->ref.REFB_R_Divider
           );
    pllPrintDPLLChannelSetup(&setup->dpll0, DPLL0);
    pllPrintDPLLChannelSetup(&setup->dpll1, DPLL1);
}

void ad9545_verbose_status(const AD9545_Status *status)
{
    printf("EEPROM status %02X %s%s%s%s\n",
           status->eeprom.raw,
           status->eeprom.b.load_in_progress ? " LOAD" : "",
           status->eeprom.b.save_in_progress ? " SAVE" : "",
           status->eeprom.b.fault ? " FAULT" : "",
           status->eeprom.b.crc_error ? " CRC error" : ""
           );
    printf("Sysclk status %02X %s%s%s%s%s\n",
           status->sysclk.raw,
           status->sysclk.b.locked ? " LOCKED" : "",
           status->sysclk.b.stable ? " STABLE" : "",
           status->sysclk.b.cal_busy ? " CAL_BUSY" : "",
           status->sysclk.b.pll0_locked ? " PLL0_LOCK" : "",
           status->sysclk.b.pll1_locked ? " PLL1_LOCK" : ""
           );
    printf("misc status %02X %s%s%s\n",
           status->misc.raw,
           status->misc.b.temp_alarm ? " TEMP_ALARM" : "",
           status->misc.b.aux_dpll_lock ? " AUX_DPLL_LOCK" : "",
           status->misc.b.aux_dpll_ref_fault ? " AUX_DPLL_REF_FAULT" : ""
           );
    printf("Internal temp %d C\n", status->internal_temp/128);
    pllPrintRefStatus(status, AD9545_REFA);
    pllPrintRefStatus(status, AD9545_REFB);
    printf(" --- DPLL channel %d ---\n", DPLL0);
    pllPrintDPLLChannelStatus(status, DPLL0);
    printf(" --- DPLL channel %d ---\n", DPLL1);
    pllPrintDPLLChannelStatus(status, DPLL1);
}

void ad9545_brief_status(const AD9545_Status *status)
{
//    printf("PLL AD9545:      %s %s",
//           ad9545_state_str(d->fsm_state),
//           sensor_status_ansi_str(get_pll_sensor_status(d)));
//    printf("%s\n", ANSI_CLEAR_EOL);
    if (1) {
        printf("  Ref A:");
        pllPrintRefStatusBits(status->ref[AD9545_REFA]);
        printf("\n");
        printf("  Ref B:");
        pllPrintRefStatusBits(status->ref[AD9545_REFB]);
        printf("\n");
        for (int channel=0; channel<DPLL_COUNT; channel++) {
            int64_t ppb0 = pll_ftw_rel_ppb(status, (AD9545_Channel_TypeDef)channel);
            const char *ref0str = "";
            AD9545_Profile_Ref_Source_TypeDef ref0 = pll_get_current_ref(status, (AD9545_Channel_TypeDef)channel);
            if (ref0 != PROFILE_REF_SOURCE_INVALID)
                ref0str = pllProfileRefSourceStr(ref0);
            bool locked = (channel == DPLL1) ? status->sysclk.b.pll1_locked : status->sysclk.b.pll0_locked;
            printf("  DPLL%d: %s ref %-5s %lld ppb",
                   channel,
                   locked ? ANSI_GREEN "LOCKED  " ANSI_CLEAR: ANSI_RED "UNLOCKED" ANSI_CLEAR,
                   ref0str,
                   (int64_t)ppb0
                   );
            printf("\n");
        }
    } else {
        for (int i=0; i<4; i++)
            printf("\n");
    }
}
