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

#if defined(BOARD_TDC72VHLV3)
const char *getPllRefDescr(int refIndex)
{
    switch (refIndex) {
    case 0:
    case 1:
        return "FPGA   125  MHz";
    case 2:
    case 3:
        return "TTC    41.7 MHz";
    case 4:
    case 5:
        return "VXS    41.7 MHz";
    case 6:
    case 7:
        return "VCXO/3 41.7 MHz";
    default:
        return "?";
    }
}
#elif defined BOARD_ADC64VE
const char *getPllRefDescr(int refIndex)
{
    return "?";
}
#else
const char *getPllRefDescr(int refIndex)
{
    return "?";
}
#endif

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

static void ad9548_print_profile(const ad9548_setup_t *setup, int index)
{
    printf(" --- Profile %d ---\n", index);
    const AD9548_Profile_TypeDef *prof = &setup->prof[index];
    printf("priority selection %d, promoted %d; phase_lock_scale %d; \
ref_period %llu; tolerance inner %d, outer %d; \
validation_timer %d; redetect_timeout %d\n",
           prof->b.selection_priority,
           prof->b.promoted_priority,
           prof->b.phase_lock_scale,
           (uint64_t)prof->b.ref_period,
           prof->b.inner_tolerance,
           prof->b.outer_tolerance,
           prof->b.validation_timer,
           prof->b.redetect_timeout);
printf("filter alpha 0_linear %d, 1_exp %d, 2_exp %d, 3_exp: %d; \
beta 0_linear %d, 1_exp %d; \
gamma 0_linear %d, 1_exp %d; \
delta 0_linear %d, 1_exp %d\n",
           prof->b.filter_alpha_0_linear,
           prof->b.filter_alpha_1_exp,
           prof->b.filter_alpha_2_exp,
           prof->b.filter_alpha_3_exp,
           prof->b.filter_beta_0_linear,
           prof->b.filter_beta_1_exp,
           prof->b.filter_gamma_0_linear,
           prof->b.filter_gamma_1_exp,
           prof->b.filter_delta_0_linear,
           prof->b.filter_delta_1_exp);
printf("R: %d, S: %d, V: %d, U: %d\n",
           prof->b.R,
           prof->b.S,
           prof->b.V,
           prof->b.U);
printf("phase_lock_threshold: %d, phase_lock_fill_rate: %d, phase_lock_drain_rate: %d\n",
           prof->b.phase_lock_threshold,
           prof->b.phase_lock_fill_rate,
       prof->b.phase_lock_drain_rate);
       printf("freq_lock_threshold: %d, freq_lock_fill_rate: %d, freq_lock_drain_rate: %d\n",
           prof->b.freq_lock_threshold,
           prof->b.freq_lock_fill_rate,
           prof->b.freq_lock_drain_rate);
}

static void ad9548_print_dpll_setup(const ad9548_setup_t *setup)
{
    const AD9548_Dpll_TypeDef *dpll = &setup->dpll;
    printf("DPLL ftw %llu; pull-in %d..%d; open loop phase %d; closed loop offs %llu, \
step %d; slew_limit %d\n",
           (uint64_t)dpll->b.ftw,
           dpll->b.pullin_lower_limit,
           dpll->b.pullin_upper_limit,
           dpll->b.dds_phase_offset_word,
           (int64_t)dpll->b.fixed_phase_lock_offset,
           dpll->b.inc_phase_lock_offset_step_size,
           dpll->b.phase_slew_limit
           );
    printf("DPLL hist: acc_timer %d, inc_avg: %d, persist: %d, ssfb: %d\n",
           dpll->b.hist_acc_timer,
           dpll->b.hist_inc_average,
           dpll->b.hist_persistent,
           dpll->b.hist_single_sample_fallback);
}

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
    ad9548_print_dpll_setup(setup);
    ad9548_print_profile(setup, 0);
    ad9548_print_profile(setup, 1);
    ad9548_print_profile(setup, 2);
    ad9548_print_profile(setup, 3);
}

void ad9548_print_pll_dpll_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    uint8_t DpllStat = status->DpllStat.raw;
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

    if (status->DpllStat2.b.freq_clamp)
        printf (ANSI_RED " | freq clamp");

    if (status->DpllStat2.b.hist_available)
        printf (ANSI_GREEN " | hist_available");

    printf(ANSI_CLEAR "\n");
//    printf(ANSI_CLEAR "  Active ref: %d, prio %d\n",
//           status->DpllStat2.b.active_ref, status->DpllStat2.b.active_ref_prio);
}

void ad9548_print_pll_ref_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    uint8_t refPowerDown = status->refPowerDown;
    uint8_t refActive = status->DpllStat2.b.active_ref;

    const char *refPinName[4] = {"A", "B", "C", "D"};

    for (int i=0; i<4; i++) {
        int curRefDown = refPowerDown & (1<<(i*2));
        int curRefActive = (refActive / 2 == i);
        AD9548_Ref_Status_REG_Type refstatus = (status->refStatus[i*2]);
        printf(ANSI_CLEAR "    " "%s %s %s",
               curRefActive ? " *" : "  ",
               refPinName[i],
               getPllRefDescr(i*2));
        if (curRefDown) {
            printf(ANSI_GRAY " disabled");
        } else {
            if (refstatus.b.valid) printf(ANSI_GREEN " VALID");
            if (refstatus.b.fault) printf(ANSI_RED   " FAULT");
            if (refstatus.b.fast) printf(ANSI_PUR   " FAST");
            if (refstatus.b.slow) printf(ANSI_PUR   " SLOW");
            if (refstatus.b.prof_selected) {
                const AD9548_Profile_TypeDef *prof = &setup->prof[refstatus.b.prof_index];
                printf(ANSI_CLEAR " profile %d, %.1f MHz",
                       refstatus.b.prof_index,
                       1e9 / prof->b.ref_period);
            }

        }
        printf(ANSI_CLEAR);
        print_clear_eol();
    }
}

void ad9548_verbose_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    printf("Sysclk status %02X %s%s%s",
           status->sysclk.raw,
           status->sysclk.b.locked ? " LOCKED" : "",
           status->sysclk.b.stable ? " STABLE" : "",
           status->sysclk.b.cal_busy ? " CAL_BUSY" : ""
           );
    print_clear_eol();
    ad9548_print_pll_dpll_status(setup, status);
    printf(ANSI_CLEAR " --- Reference inputs --- ");
    print_clear_eol();
    ad9548_print_pll_ref_status(setup, status);
    printf("Holdover FTW: %lld\n", (uint64_t)status->holdover_ftw);
}

void ad9548_brief_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    ad9548_print_pll_dpll_status(setup, status);
    ad9548_print_pll_ref_status(setup, status);

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
