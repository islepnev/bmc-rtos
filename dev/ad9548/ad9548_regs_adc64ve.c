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

#include "ad9548_regs_adc64ve.h"

const AD9548_Output_TypeDef PLL_Output_ADC64VE = {
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
    .b.sync_source = 2,
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
    .b.q_1 = 1,
    .b.q_2 = 1,
    .b.q_3 = 0
};

const AD9548_Profile_TypeDef PLL_Prof0_ADC64VE = {
    .b.selection_priority=3,
    .b.promoted_priority=3,
    .b.ref_period=16000000,
    .b.inner_tolerance=1000,
    .b.outer_tolerance=1000,
    .b.filter_alpha_0_linear=39705,
    .b.filter_alpha_1_exp=5,
    .b.filter_alpha_2_exp=0,
    .b.filter_beta_0_linear=81944,
    .b.filter_beta_1_exp=13,
    .b.filter_gamma_0_linear=86346,
    .b.filter_gamma_1_exp=13,
    .b.filter_delta_0_linear=28989,
    .b.filter_delta_1_exp=12,
    .b.filter_alpha_3_exp=0,
    .b.R=29,
    .b.S=59,
    .b.phase_lock_threshold=100,
    .b.phase_lock_fill_rate=32,
    .b.phase_lock_drain_rate=68,
    .b.freq_lock_threshold=100,
    .b.freq_lock_fill_rate=32,
    .b.freq_lock_drain_rate=68
};

const AD9548_Profile_TypeDef PLL_Prof1_ADC64VE = {
    .b.selection_priority=0,
    .b.promoted_priority=0,
    .b.ref_period=24000000,
    .b.ref_period_padding=0,
    .b.inner_tolerance=200,
    .b.outer_tolerance=10,
    .b.filter_alpha_0_linear=39705,
    .b.filter_alpha_1_exp=5,
    .b.filter_alpha_2_exp=0,
    .b.filter_beta_0_linear=81944,
    .b.filter_beta_1_exp=13,
    .b.filter_gamma_0_linear=86346,
    .b.filter_gamma_1_exp=13,
    .b.filter_delta_0_linear=28989,
    .b.filter_delta_1_exp=12,
    .b.filter_alpha_3_exp=0,
    .b.R=19,
    .b.S=59,
    .b.phase_lock_threshold=100,
    .b.phase_lock_fill_rate=32,
    .b.phase_lock_drain_rate=68,
    .b.freq_lock_threshold=100,
    .b.freq_lock_fill_rate=32,
    .b.freq_lock_drain_rate=68
};

const AD9548_Profile_TypeDef PLL_Prof2_ADC64VE = {
    .b.selection_priority=0,
    .b.promoted_priority=0,
    .b.ref_period=48000000,
    .b.ref_period_padding=0,
    .b.inner_tolerance=1000,
    .b.outer_tolerance=200,
    .b.filter_alpha_0_linear=39705,
    .b.filter_alpha_1_exp=5,
    .b.filter_alpha_2_exp=0,
    .b.filter_beta_0_linear=81944,
    .b.filter_beta_1_exp=13,
    .b.filter_gamma_0_linear=86346,
    .b.filter_gamma_1_exp=13,
    .b.filter_delta_0_linear=28989,
    .b.filter_delta_1_exp=12,
    .b.filter_alpha_3_exp=0,
    .b.R=9,
    .b.S=59,
    .b.phase_lock_threshold=10000,
    .b.phase_lock_fill_rate=32,
    .b.phase_lock_drain_rate=68,
    .b.freq_lock_threshold=500,
    .b.freq_lock_fill_rate=32,
    .b.freq_lock_drain_rate=68
};

const AD9548_Profile_TypeDef PLL_Prof3_ADC64VE = {
    .b.selection_priority=1,
    .b.promoted_priority=1,
    .b.ref_period=16000000,
    .b.ref_period_padding=0,
    .b.inner_tolerance=1000,
    .b.outer_tolerance=1000,
    .b.filter_alpha_0_linear=39705,
    .b.filter_alpha_1_exp=5,
    .b.filter_alpha_2_exp=0,
    .b.filter_beta_0_linear=81944,
    .b.filter_beta_1_exp=13,
    .b.filter_gamma_0_linear=86346,
    .b.filter_gamma_1_exp=13,
    .b.filter_delta_0_linear=28989,
    .b.filter_delta_1_exp=12,
    .b.filter_alpha_3_exp=0,
    .b.R=29,
    .b.S=59,
    .b.phase_lock_threshold=100,
    .b.phase_lock_fill_rate=32,
    .b.phase_lock_drain_rate=68,
    .b.freq_lock_threshold=100,
    .b.freq_lock_fill_rate=32,
    .b.freq_lock_drain_rate=68
};
