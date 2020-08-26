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

#include "ad9548_print.h"

#include <stdio.h>
#include <stdbool.h>

#include "ad9548_setup.h"
#include "ad9548_setup_regs.h"
#include "ad9548_status_regs.h"
#include "display.h"

const char *getPllRefDescr(int refIndex)
{
    switch (refIndex) {
    case 0:
    case 1:
        return "FPGA 125  MHz";
    case 2:
    case 3:
        return "TTC  20.8 MHz";
    case 4:
    case 5:
        return "VXS  125  MHz";
    case 6:
    case 7:
        return "VCXO 62.5 MHz";
    default:
        return "";
    }
}

//static int64_t pll_ftw_rel_ppb(const AD9548_Status *status)
//{
//    const DPLL_Status *dpll_status = &status->dpll;
//    uint64_t ftw = dpll_status->ftw_history;
//    uint64_t default_ftw = get_dpll_default_ftw();
//    int64_t twdelta = ftw - default_ftw;
//    int64_t norm = default_ftw/1000000000ULL;
//    return twdelta/norm;
//}

//static ProfileRefSource_TypeDef pll_get_current_ref(const AD9548_Status *status)
//{
//    //    int active = d->status.dpll[channel].operation.b.active;
//    //    if (!active)
//    //        return PROFILE_REF_SOURCE_INVALID;
//    uint8_t active_profile = status->dpll.operation.b.active_profile;
//    if (active_profile >= AD9548_DPLL_PROFILE_COUNT)
//        return PROFILE_REF_SOURCE_INVALID;
//    Pll_DPLL_Setup_TypeDef dpll = {0};
//    init_DPLL_Setup(&dpll, channel);
//    return (ProfileRefSource_TypeDef)dpll.profile[active_profile].Profile_Ref_Source;
//}

//static void pllPrintRefStatusBits(Ref_Status_REG_Type r)
//{
//    printf("%s%s%s%s%s%s",
//           r.b.slow   ? " SLOW" : "",
//           r.b.fast   ? " FAST" : "",
//           r.b.jitter ? " JITTER" : "",
//           r.b.fault  ? " FAULT" : "",
//           r.b.valid  ? " VALID" : "",
//           r.b.los    ? " LOS" : ""
//           );
//}

//static const char *pllProfileRefSourceStr(ProfileRefSource_TypeDef r)
//{
//    switch(r) {
//    case PROFILE_REF_SOURCE_A: return "A";
//    case PROFILE_REF_SOURCE_AA: return "AA";
//    case PROFILE_REF_SOURCE_B: return "B";
//    case PROFILE_REF_SOURCE_BB: return "BB";
//    case PROFILE_REF_SOURCE_DPLL0: return "DPLL0";
//    case PROFILE_REF_SOURCE_DPLL1: return "DPLL1";
//    case PROFILE_REF_SOURCE_NCO0: return "NCO0";
//    case PROFILE_REF_SOURCE_NCO1: return "NCO1";
//    case PROFILE_REF_SOURCE_INVALID: return "";
//    default: return "";
//    }
//}

//static void pllPrintRefStatus(const AD9548_Status *status, PllRef_TypeDef ref_input)
//{
//    Ref_Status_REG_Type r = status->ref[ref_input];
//    printf("Ref %d  %02X",
//           ref_input,
//           r.raw
//           );
//    pllPrintRefStatusBits(r);
//    printf("\n");
//}

//static void pllPrintDPLLStatus(const AD9548_Status *status)
//{
//    const DPLL_Status *dpll_status = &status->dpll[channel];
//    {
//        DPLL_Active_Profile_REG_Type r = dpll_status->act_profile;
//        printf("Translation profile %d.%s%s%s%s%s%s%s\n",
//               channel,
//               ((r.raw & 0x3F) ==0) ? "none" : "",
//               r.b.profile_0 ? "0" : "",
//               r.b.profile_1 ? "1" : "",
//               r.b.profile_2 ? "2" : "",
//               r.b.profile_3 ? "3" : "",
//               r.b.profile_4 ? "4" : "",
//               r.b.profile_5 ? "5" : ""
//                               );
//    }
//    {
//        DPLL_Lock_Status_REG_Type r = dpll_status->lock_status;
//        printf("Lock status %02X %s%s%s%s%s%s\n",
//               r.raw,
//               r.b.all_lock ? " ALL_LOCK" : "",
//               r.b.dpll_phase_lock ? " D_PHASE_LOCK" : "",
//               r.b.dpll_freq_lock ? " D_FREQ_LOCK" : "",
//               r.b.apll_lock ? " A_LOCK" : "",
//               r.b.apll_cal_busy ? " A_CAL_BUSY" : "",
//               r.b.apll_cal_done ? " A_CALIBRATED" : ""
//                                   );
//    }
//    {
//        DPLL_Operation_REG_Type r = dpll_status->operation;
//        printf("Oper status %02X %s%s%s%s %s %d.%d\n",
//               r.raw,
//               r.b.freerun ? " FREERUN" : "",
//               r.b.holdover ? " HOLDOVER" : "",
//               r.b.ref_switch ? " REF_SWITCH" : "",
//               r.b.active ? " ACTIVE" : "",
//               r.b.active ? "current profile" : "last profile",
//               channel,
//               r.b.active_profile
//               );
//    }
//    {
//        DPLL_State_REG_Type r = dpll_status->state;
//        printf("State %02X %s%s%s%s%s\n",
//               r.raw,
//               r.b.hist_available   ? " HIST" : "",
//               r.b.freq_clamp       ? " FREQ_CLAMP" : "",
//               r.b.phase_slew_limit ? " PHASE_SLEW_LIMIT" : "",
//               r.b.facq_active      ? " FACQ_ACT" : "",
//               r.b.facq_done        ? " FACQ_DONE" : ""
//                                      );
//    }
////    {
////        uint64_t ftw = dpll_status->ftw_history;
////        double ppb = pll_ftw_rel_ppb(status, channel);
////        if (dpll_status->state.b.freq_clamp)
////            printf("TW history %lld (FREQ_CLAMP)\n", ftw);
////        else
////            printf("TW history %lld (%lld ppb)\n", ftw, (int64_t) ppb);
////    }

//    printf("PLD tub %d, FLD tub %d\n", dpll_status->pld_tub, dpll_status->fld_tub);

//    printf("Phase slew %02X %s%s%s%s\n",
//           dpll_status->phase_slew,
//           (dpll_status->phase_slew & 0x1) ? " Q1A" : "",
//           (dpll_status->phase_slew & 0x2) ? " Q1AA" : "",
//           (dpll_status->phase_slew & 0x4) ? " Q1B" : "",
//           (dpll_status->phase_slew & 0x8) ? " Q1BB" : ""
//                                             );
//    printf("Phase control error %02X %s%s%s%s\n",
//           dpll_status->phase_control_error,
//           (dpll_status->phase_control_error & 0x1) ? " Q1A" : "",
//           (dpll_status->phase_control_error & 0x2) ? " Q1AA" : "",
//           (dpll_status->phase_control_error & 0x4) ? " Q1B" : "",
//           (dpll_status->phase_control_error & 0x8) ? " Q1BB" : ""
//                                                      );
//}

//static char *ad9548_state_text(ad9548_state_t pllState)
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

//static void pllPrintDPLLSetup(const AD9548_DPLL_Setup_TypeDef *dpll)
//{

//    printf("DPLL%d profile[0] %s%s%s%s, FB %s%s FBdiv %u+%u/%u, BW %g Hz\n",
//           channel,
//           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_A) ? "RefA" : "",
//           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_B) ? "RefB" : "",
//           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_DPLL0) ? "DPLL0" : "",
//           (dpll->profile[0].Profile_Ref_Source == PROFILE_REF_SOURCE_DPLL1) ? "DPLL1" : "",
//           (dpll->profile[0].ZD_Feedback_Path == PROFILE_EXT_ZD_FEEDBACK_REFA) ? "RefA" : "",
//           (dpll->profile[0].ZD_Feedback_Path == PROFILE_EXT_ZD_FEEDBACK_REFB) ? "RefB" : "",
//           dpll->profile[0].Buildout_FB_Divider,
//           dpll->profile[0].Buildout_FB_Fraction,
//           dpll->profile[0].Buildout_FB_Modulus,
//           dpll->profile[0].Loop_BW / 1e6
//           );

//}

void ad9548_verbose_setup(const ad9548_setup_t *setup)
{
//    /*
//    Pll_OutputDrivers_Setup_TypeDef out_drivers;
//    Pll_OutputDividers_Setup_TypeDef out_dividers;
//    Pll_DPLLMode_Setup_TypeDef dpll_mode;
//    PllSysclkSetup_TypeDef sysclk;
//*/
//    printf("RefA: % 3g ns, Rdiv %u\n",
//           setup->ref.REFA_Input_Period * 1e-9,
//           setup->ref.REFA_R_Divider
//           );
//    printf("RefB: % 3g ns, Rdiv %u\n",
//           setup->ref.REFB_Input_Period * 1e-9,
//           setup->ref.REFB_R_Divider
//           );
//    pllPrintDPLLSetup(&setup->dpll);
}

void ad9548_print_pll_dpll_status(uint8_t DpllStat)
{
    // DPLL status
    printf(ANSI_CLEAR "  DPLL: %02X ", DpllStat);

    if ((DpllStat >> 5) & 1)
        printf (ANSI_GREEN "frequency lock");
    else
        printf (ANSI_RED "no frequency lock");
    printf(" | ");

    if ((DpllStat >> 4) & 1)
        printf (ANSI_GREEN "phase lock");
    else
        printf (ANSI_RED "no phase lock");

    if ((DpllStat >> 7) & 1)
        printf (ANSI_RED " | offset slew limiting");

    if ((DpllStat >> 6) & 1)
        printf (ANSI_RED " | phase build-out");

    if ((DpllStat >> 3) & 1)
        printf (ANSI_RED " | reference switchover");

    if ((DpllStat >> 2) & 1)
        printf (ANSI_RED " | holdover mode");

    if ((DpllStat >> 1) & 1)
        printf (ANSI_GREEN " | active (closed-loop)");

    if ((DpllStat >> 0) & 1)
        printf (ANSI_RED " | free running (open-loop)");

    printf(ANSI_CLEAR "\n");
}

void ad9548_print_pll_ref_status(const AD9548_Status *pllStatusRegs)
{
    printf(ANSI_CLEAR "  Reference inputs");
    print_clear_eol();
    uint8_t refPowerDown = pllStatusRegs->refPowerDown;
    uint8_t refActive = pllStatusRegs->refActive;

    const char *refPinName[4] = {"A", "B", "C", "D"};

    for (int i=0; i<4; i++) {
        int curRefDown = refPowerDown & (1<<(i*2));
        int curRefActive = (refActive / 2 == i);
        uint8_t status = (pllStatusRegs->refStatus[i*2]);
        printf(ANSI_CLEAR "    " "%s %s %s",
               curRefActive ? " *" : "  ",
               refPinName[i],
               getPllRefDescr(i*2));
        if (curRefDown) {
            printf(ANSI_GRAY " disabled");
        } else {
            if ((status >> 3) & 1) printf(ANSI_GREEN " VALID");
            if ((status >> 2) & 1) printf(ANSI_RED   " FAULT");
            if ((status >> 1) & 1) printf(ANSI_PUR   " FAST");
            if ((status >> 0) & 1) printf(ANSI_PUR   " SLOW");
        }
        printf(ANSI_CLEAR);
        print_clear_eol();
    }
}

void ad9548_verbose_status(const AD9548_Status *status)
{
    printf("Sysclk status %02X %s%s%s",
           status->sysclk.raw,
           status->sysclk.b.locked ? " LOCKED" : "",
           status->sysclk.b.stable ? " STABLE" : "",
           status->sysclk.b.cal_busy ? " CAL_BUSY" : ""
           );
    print_clear_eol();
    ad9548_print_pll_dpll_status(status->DpllStat.raw);
    ad9548_print_pll_ref_status(status);
}

void ad9548_brief_status(const AD9548_Status *status)
{
    ad9548_print_pll_dpll_status(status->DpllStat.raw);
////    printf("PLL AD9548:      %s %s",
////           ad9548_state_str(d->fsm_state),
////           sensor_status_ansi_str(get_pll_sensor_status(d)));
////    printf("%s\n", ANSI_CLEAR_EOL);
//    if (1) {
//        printf("  Ref A:");
//        pllPrintRefStatusBits(status->ref[AD9548_REFA]);
//        printf("%s\n", ANSI_CLEAR_EOL);
//        printf("  Ref B:");
//        pllPrintRefStatusBits(status->ref[REFB]);
//        printf("%s\n", ANSI_CLEAR_EOL);
//        for (int channel=0; channel<DPLL_COUNT; channel++) {
//            int64_t ppb0 = pll_ftw_rel_ppb(status, (PllChannel_TypeDef)channel);
//            const char *ref0str = "";
//            ProfileRefSource_TypeDef ref0 = pll_get_current_ref(status, (PllChannel_TypeDef)channel);
//            if (ref0 != PROFILE_REF_SOURCE_INVALID)
//                ref0str = pllProfileRefSourceStr(ref0);
//            bool locked = (channel == DPLL1) ? status->sysclk.b.pll1_locked : status->sysclk.b.pll0_locked;
//            printf("  DPLL%d: %s ref %-5s %lld ppb",
//                   channel,
//                   locked ? ANSI_GREEN "LOCKED  " ANSI_CLEAR: ANSI_RED "UNLOCKED" ANSI_CLEAR,
//                   ref0str,
//                   (int64_t)ppb0
//                   );
//            printf("%s\n", ANSI_CLEAR_EOL);
//        }
//    } else {
//        for (int i=0; i<4; i++)
//            printf("%s\n", ANSI_CLEAR_EOL);
//    }
}
