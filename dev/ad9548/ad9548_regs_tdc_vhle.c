/*
**    Copyright 2020 Ilja Slepnev
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

#include "ad9548_regs_tdc_vhle.h"

const AD9548_Output_TypeDef PLL_Output_TDC_VHLE = {
    .b.powerdown_0 = 0,
    .b.powerdown_1 = 0,
    .b.powerdown_2 = 0,
    .b.powerdown_3 = 0,
    .b.receiver_mode = 0,
    .b.ext_distrib_resistor = 0,
    .b.en_0 = 1,
    .b.en_1 = 1,
    .b.en_2 = 1,
    .b.en_3 = 1,
    .b.sync_mask_0 = 0,
    .b.sync_mask_1 = 0,
    .b.sync_mask_2 = 0,
    .b.sync_mask_3 = 0,
    .b.sync_source = 1,
    .b.autosync_mode = 2,
    .b.out_0_mode = 4,
    .b.out_0_drive = 0,
    .b.out_0_invert = 0,
    .b.out_0_cmos_invert = 0,
    .b.out_1_mode = 4,
    .b.out_1_drive = 0,
    .b.out_1_invert = 0,
    .b.out_1_cmos_invert = 0,
    .b.out_2_mode = 4,
    .b.out_2_drive = 0,
    .b.out_2_invert = 0,
    .b.out_2_cmos_invert = 0,
    .b.out_3_mode = 4,
    .b.out_3_drive = 1,
    .b.out_3_invert = 0,
    .b.out_3_cmos_invert = 0,
    .b.q_0 = 0,
    .b.q_1 = 0,
    .b.q_2 = 2,
    .b.q_3 = 2
};

// 125 MHz lowest priority
const AD9548_Profile_TypeDef *PLL_Prof0_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    PLL_Prof_default(p);
    p->b.selection_priority=3;
    p->b.promoted_priority=3;
    p->b.ref_period=8000000;
    p->b.inner_tolerance=1000;
    p->b.outer_tolerance=200;
    p->b.filter_alpha_0_linear=52940;
    p->b.filter_alpha_1_exp=7;
    p->b.filter_alpha_2_exp=0;
    p->b.filter_beta_0_linear=81944;
    p->b.filter_beta_1_exp=13;
    p->b.filter_gamma_0_linear=86346;
    p->b.filter_gamma_1_exp=13;
    p->b.filter_delta_0_linear=28989;
    p->b.filter_delta_1_exp=12;
    p->b.filter_alpha_3_exp=0;
    p->b.R=19;
    p->b.S=19;
    p->b.phase_lock_threshold=10000;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=500;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    return p;
};

// 41.7 MHz
const AD9548_Profile_TypeDef *PLL_Prof1_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    p->b.selection_priority=0;
    p->b.promoted_priority=0;
    p->b.ref_period=24000000;
    p->b.ref_period_padding=0;
    p->b.inner_tolerance=1000;
    p->b.outer_tolerance=1000;
    p->b.filter_alpha_0_linear=62039;
    p->b.filter_alpha_1_exp=0;
    p->b.filter_alpha_2_exp=0;
    p->b.filter_beta_0_linear=81944;
    p->b.filter_beta_1_exp=13;
    p->b.filter_gamma_0_linear=86346;
    p->b.filter_gamma_1_exp=13;
    p->b.filter_delta_0_linear=28989;
    p->b.filter_delta_1_exp=12;
    p->b.filter_alpha_3_exp=0;
    p->b.R=999;
    p->b.S=2999;
    p->b.phase_lock_threshold=100;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=100;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    return p;
};

// 40 MHz
const AD9548_Profile_TypeDef *PLL_Prof2_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    p->b.selection_priority=0;
    p->b.promoted_priority=0;
    p->b.ref_period=25000000;
    p->b.ref_period_padding=0;
    p->b.inner_tolerance=1000;
    p->b.outer_tolerance=1000;
    p->b.filter_alpha_0_linear=33087;
    p->b.filter_alpha_1_exp=5;
    p->b.filter_alpha_2_exp=0;
    p->b.filter_beta_0_linear=81944;
    p->b.filter_beta_1_exp=13;
    p->b.filter_gamma_0_linear=86346;
    p->b.filter_gamma_1_exp=13;
    p->b.filter_delta_0_linear=28989;
    p->b.filter_delta_1_exp=12;
    p->b.filter_alpha_3_exp=0;
    p->b.R=15;
    p->b.S=49;
    p->b.phase_lock_threshold=100;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=100;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    return p;
};

// 41.7 MHz
const AD9548_Profile_TypeDef *PLL_Prof3_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    p->b.selection_priority=2;
    p->b.promoted_priority=2;
    p->b.ref_period=24000000;
    p->b.ref_period_padding=0;
    p->b.inner_tolerance=1000;
    p->b.outer_tolerance=1000;
    p->b.filter_alpha_0_linear=39705;
    p->b.filter_alpha_1_exp=6;
    p->b.filter_alpha_2_exp=0;
    p->b.filter_beta_0_linear=81944;
    p->b.filter_beta_1_exp=13;
    p->b.filter_gamma_0_linear=86346;
    p->b.filter_gamma_1_exp=13;
    p->b.filter_delta_0_linear=28989;
    p->b.filter_delta_1_exp=12;
    p->b.filter_alpha_3_exp=0;
    p->b.R=9;
    p->b.S=29;
    p->b.phase_lock_threshold=100;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=100;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    return p;
};
