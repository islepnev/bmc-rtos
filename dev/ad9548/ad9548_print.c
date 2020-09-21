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
#include "board_config_ad9548.h"
#include "display.h"
#include "log/log.h"

//static int64_t pll_ftw_rel_ppb(const AD9548_Status *status)
//{
//    const DPLL_Status *dpll_status = &status->dpll;
//    uint64_t ftw = dpll_status->ftw_history;
//    uint64_t default_ftw = get_dpll_default_ftw();
//    int64_t twdelta = ftw - default_ftw;
//    int64_t norm = default_ftw/1000000000ULL;
//    return twdelta/norm;
//}

double ad9548_profile_dds_freq(const AD9548_Profile_TypeDef *prof)
{
    return 1e15 / prof->b.ref_period / (prof->b.R+1) *
           (prof->b.S+1+prof->b.U/prof->b.V);
}

double ad9548_profile_tdc_freq(const AD9548_Profile_TypeDef *prof)
{
    return 1e15 / prof->b.ref_period / (prof->b.R+1);
}

static void ad9548_print_profile(const ad9548_setup_t *setup, int index)
{
    printf(" --- Profile %d ---\n", index);
    const AD9548_Profile_TypeDef *prof = &setup->prof[index];
    printf("priority sel %d, prom %d; phase_lock_scale %d; \
ref_period %.0f ns; tolerance inner %.0f ppm, outer %.0f ppm; \
validation %d ms; redetect %d ms\n",
           prof->b.selection_priority,
           prof->b.promoted_priority,
           prof->b.phase_lock_scale,
           (uint64_t)prof->b.ref_period / 1e6,
           1e6 / prof->b.inner_tolerance,
           1e6 / prof->b.outer_tolerance,
           prof->b.validation_timer,
           prof->b.redetect_timeout);
    // printf("filter a0 %d, a1 %d, a2 %d, a3 %d; b0 %d, b1 %d; g0 %d, g1 %d; d0 %d, d1 %d\n",
    //       prof->b.filter_alpha_0_linear,
    //       prof->b.filter_alpha_1_exp,
    //       prof->b.filter_alpha_2_exp,
    //       prof->b.filter_alpha_3_exp,
    //       prof->b.filter_beta_0_linear,
    //       prof->b.filter_beta_1_exp,
    //       prof->b.filter_gamma_0_linear,
    //       prof->b.filter_gamma_1_exp,
    //       prof->b.filter_delta_0_linear,
    //       prof->b.filter_delta_1_exp);
    printf("R: %d, S: %d, U: %d, V: %d\n",
           prof->b.R, prof->b.S, prof->b.U, prof->b.V);
    printf("phase_lock: %.0f ps, fill %d, drain %d, freq_lock: %d ps, fill %d, drain %d\n",
           prof->b.phase_lock_threshold * (prof->b.phase_lock_scale ? 1e3 : 1),
           prof->b.phase_lock_fill_rate,
           prof->b.phase_lock_drain_rate,
           prof->b.freq_lock_threshold,
           prof->b.freq_lock_fill_rate,
           prof->b.freq_lock_drain_rate);
    double f_dds = ad9548_profile_dds_freq(prof);
    double f_tdc = ad9548_profile_tdc_freq(prof);
    printf("fDDS %.3f MHz, fTDC %.3f MHz\n", f_dds/1e6, f_tdc/1e6);
}

static void ad9548_print_sysclk_setup(const ad9548_setup_t *setup)
{
    const AD9548_Sysclk_TypeDef *sysclk = &setup->sysclk;
    printf("Sysclk: %.3f MHz\n",
           1e9 / sysclk->b.sysclk_period_fs.b.value);
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

static void ad9548_print_output_setup(const ad9548_setup_t *setup)
{
    const AD9548_Output_TypeDef *output = &setup->output;
    printf("Output: powerdown %d %d %d %d; ",
           output->b.powerdown_0, output->b.powerdown_1,
           output->b.powerdown_2, output->b.powerdown_3);
    // printf("rx_mode %d; ", output->b.receiver_mode);
    // printf("ext_distrib_resistor=%d\n", output->b.ext_distrib_resistor);
    printf("en %d %d %d %d; ", output->b.en_0, output->b.en_1,
           output->b.en_2, output->b.en_3);
    printf("sync_mask %d %d %d %d; ", output->b.sync_mask_0, output->b.sync_mask_1,
           output->b.sync_mask_2, output->b.sync_mask_3);
    printf("sync_source %d; ", output->b.sync_source);
    printf("autosync_mode %d\n", output->b.autosync_mode);
    printf("Q0 DIV %d, mode %d, drive %d, invert %d, cmos_invert %d\n",
           output->b.q_0 + 1,
           output->b.out_0_mode, output->b.out_0_drive,
           output->b.out_0_invert, output->b.out_0_cmos_invert);
    printf("Q1 DIV %d, mode %d, drive %d, invert %d, cmos_invert %d\n",
           output->b.q_1 + 1,
           output->b.out_1_mode, output->b.out_1_drive,
           output->b.out_1_invert, output->b.out_1_cmos_invert);
    printf("Q2 DIV %d, mode %d, drive %d, invert %d, cmos_invert %d\n",
           output->b.q_2 + 1,
           output->b.out_2_mode, output->b.out_2_drive,
           output->b.out_2_invert, output->b.out_2_cmos_invert);
    printf("Q3 DIV %d, mode %d, drive %d, invert %d, cmos_invert %d\n",
           output->b.q_3 + 1,
           output->b.out_3_mode, output->b.out_3_drive,
           output->b.out_3_invert, output->b.out_3_cmos_invert);
}

static void ad9548_print_irq_setup(const ad9548_setup_t *setup)
{
    const AD9548_IRQMask_TypeDef *irqmask = &setup->irqmask;
    printf("IRQ mask:");
    for (int i=0; i<PLL_IRQMASK_SIZE; i++)
        printf(" %02X", irqmask->v[i]);
    printf("\n");
}

void ad9548_print_irq_status(const AD9548_IRQMask_TypeDef *irq)
{
    //    for (int i=0; i<8; i++) {
    //        if (irq_flags.v[i] & ~0/*reg->irqmask.v[i]*/) {
    //            log_printf(LOG_INFO, "irq[%d] %02X", i, irq_flags.v[i]);
    //        }
    //    }
    if (irq->v[0])
        log_printf(
            irq->b.sysclk_unlocked ? LOG_ERR : LOG_INFO,
            "ad9548 sysclk:%s%s%s%s",
            irq->b.sysclk_cal_started ? " cal_started" : "",
            irq->b.sysclk_cal_complete ? " cal_complete" : "",
            irq->b.sysclk_locked ? " locked" : "",
            irq->b.sysclk_unlocked ? " unlocked" : ""
            );
    if (irq->v[1])
        log_printf(
            LOG_INFO,
            "ad9548:%s%s%s%s",
            irq->b.eeprom_complete ? " eeprom_complete" : "",
            irq->b.eeprom_fault ? " eeprom_fault" : "",
            irq->b.watchdog_timer ? " watchdog_timer" : "",
            irq->b.dist_sync ? " dist_sync" : ""
            );
    if (irq->v[2])
        log_printf(
            (irq->b.dpll_phase_unlocked || irq->b.dpll_freq_unlocked)
                ? LOG_WARNING : LOG_INFO,
            "ad9548 dpll:%s%s%s%s%s%s%s%s",
            irq->b.dpll_phase_locked ? " phase_locked" : "",
            irq->b.dpll_phase_unlocked ? " phase_unlocked" : "",
            irq->b.dpll_freq_locked ? " freq_locked" : "",
            irq->b.dpll_freq_unlocked ? " freq_unlocked" : "",
            irq->b.dpll_holdover ? " holdover" : "",
            irq->b.dpll_freerun ? " freerun" : "",
            irq->b.dpll_closed ? " closed_loop" : "",
            irq->b.dpll_switching ? " switching" : ""
            );
    if (irq->v[3])
        log_printf(
            (irq->b.phase_slew_limited || irq->b.freq_clamped)
                ? LOG_WARNING : LOG_INFO,
            "ad9548:%s%s%s%s%s",
            irq->b.phase_slew_limited ? " phase_slew_limited" : "",
            irq->b.phase_slew_unlimited ? " phase_slew_unlimited" : "",
            irq->b.freq_clamped ? " freq_clamped" : "",
            irq->b.freq_unclamped ? " freq_unclamped" : "",
            irq->b.hist_updated ? " hist_updated" : ""
            );
    if (irq->v[4])
        log_printf(
            (irq->b.ref_a_fault)
                ? LOG_WARNING : LOG_INFO,
            "ad9548 ref_a:%s%s%s%s",
            irq->b.ref_a_fault ? " fault" : "",
            irq->b.ref_a_fault_cleared ? " fault_cleared" : "",
            irq->b.ref_a_validated ? " validated" : "",
            irq->b.ref_a_new_profile ? " new_profile" : ""
            );
    if (irq->v[5])
        log_printf(
            (irq->b.ref_b_fault)
                ? LOG_WARNING : LOG_INFO,
            "ad9548 ref_b:%s%s%s%s",
            irq->b.ref_b_fault ? " fault" : "",
            irq->b.ref_b_fault_cleared ? " fault_cleared" : "",
            irq->b.ref_b_validated ? " validated" : "",
            irq->b.ref_b_new_profile ? " new_profile" : ""
            );
    if (irq->v[6])
        log_printf(
            (irq->b.ref_c_fault)
                ? LOG_WARNING : LOG_INFO,
            "ad9548 ref_c:%s%s%s%s",
            irq->b.ref_c_fault ? " fault" : "",
            irq->b.ref_c_fault_cleared ? " fault_cleared" : "",
            irq->b.ref_c_validated ? " validated" : "",
            irq->b.ref_c_new_profile ? " new_profile" : ""
            );
    if (irq->v[7])
        log_printf(
            (irq->b.ref_d_fault)
                ? LOG_WARNING : LOG_INFO,
            "ad9548 ref_d:%s%s%s%s",
            irq->b.ref_d_fault ? " fault" : "",
            irq->b.ref_d_fault_cleared ? " fault_cleared" : "",
            irq->b.ref_d_validated ? " validated" : "",
            irq->b.ref_d_new_profile ? " new_profile" : ""
            );
}

void ad9548_verbose_setup(const ad9548_setup_t *setup)
{
    ad9548_print_sysclk_setup(setup);
    ad9548_print_dpll_setup(setup);
    for (int i=0; i<AD9548_DPLL_PROFILE_COUNT; i++)
        if (setup->prof[i].b.ref_period > 0)
            ad9548_print_profile(setup, i);
    ad9548_print_output_setup(setup);
    ad9548_print_irq_setup(setup);
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

static bool is_ref_profile_manual(const ad9548_setup_t *setup, int ref_index)
{
    switch (ref_index) {
    case 0: return setup->refin.b.manual_select.b.en_a;
    case 1: return setup->refin.b.manual_select.b.en_aa;
    case 2: return setup->refin.b.manual_select.b.en_b;
    case 3: return setup->refin.b.manual_select.b.en_bb;
    case 4: return setup->refin.b.manual_select.b.en_c;
    case 5: return setup->refin.b.manual_select.b.en_cc;
    case 6: return setup->refin.b.manual_select.b.en_d;
    case 7: return setup->refin.b.manual_select.b.en_dd;
    }
    return false;
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
               ad9548_ref_label(i*2));
        if (curRefDown) {
            printf(ANSI_GRAY " disabled");
        } else {
            if (refstatus.b.valid) printf(ANSI_GREEN " VALID");
            if (refstatus.b.fault) printf(ANSI_RED   " FAULT");
            if (refstatus.b.fast) printf(ANSI_PUR   " FAST");
            if (refstatus.b.slow) printf(ANSI_PUR   " SLOW");
            if (refstatus.b.prof_selected) {
                const AD9548_Profile_TypeDef *prof = &setup->prof[refstatus.b.prof_index];
                printf(ANSI_CLEAR " %s profile %d (prio %d), %.1f MHz",
                       is_ref_profile_manual(setup, i*2) ? "force" : "auto ",
                       refstatus.b.prof_index,
                       prof->b.promoted_priority,
                       1e9 / prof->b.ref_period);
            }

        }
        printf(ANSI_CLEAR "\n");
    }
}

static double fdds(void) {
    return 1e15 / ad9548_dds_period_fs;
}

double ad9548_ftw_offset(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    double fs = 1e15 / setup->sysclk.b.sysclk_period_fs.b.value;
    double f_hold = fs * status->holdover_ftw / ((uint64_t)1<<48);
    return f_hold-fdds();
}

void ad9548_print_ftw(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    //    int active_ref = status->DpllStat2.b.active_ref;
    //    bool profile_selected = status->refStatus[active_ref].b.prof_selected;
    //    int profile_index = status->refStatus[active_ref].b.prof_index;
    //    double fdds = ad9548_profile_dds_freq(&setup->prof[profile_index]);
    double ftw_offset_hz = ad9548_ftw_offset(setup, status);
    printf("  Holdover: %+.1f Hz, %+.3f ppm\n",
           ftw_offset_hz,
           ftw_offset_hz/fdds()*1e6);
}

void ad9548_verbose_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    printf("Sysclk status %02X %s%s%s",
           status->sysclk.raw,
           status->sysclk.b.locked ? " LOCKED" : "",
           status->sysclk.b.stable ? " STABLE" : "",
           status->sysclk.b.cal_busy ? " CAL_BUSY" : ""
           );
    printf("\n");
    ad9548_print_pll_dpll_status(setup, status);
    printf(" --- Reference inputs --- ");
    printf("\n");
    ad9548_print_pll_ref_status(setup, status);
    ad9548_print_ftw(setup, status);
}

void ad9548_brief_status(const ad9548_setup_t *setup, const AD9548_Status *status)
{
    ad9548_print_pll_dpll_status(setup, status);
    ad9548_print_pll_ref_status(setup, status);
    ad9548_print_ftw(setup, status);
}
