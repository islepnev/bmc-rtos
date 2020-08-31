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

#include "ad9548_regs_profile.h"

#include <math.h>

#include "board_config_ad9548.h"
#include "log/log.h"
#include "debug_helpers.h"

void ad9548_profile_find_r_s(AD9548_Profile_TypeDef *p, double ref_MHz)
{
    int pr = 1;
    int ps = 1;
    double ref_period_fs = 1e9 / ref_MHz;
    double ratio = (double) ad9548_dds_period_fs / ref_period_fs;
    double min = 1;
    for (int r=1; r<1000; r++) {
        double f_tdc = 1e15 / ref_period_fs / r;
        if (f_tdc > 10e6) // limit 10 MHz
            continue;
        if (f_tdc > 1.0e5) // test
            continue;
        int s = lround(r / ratio);
        double diff = fabs(ratio - 1.0 * r / s);
        if (diff < min) {
            min = diff;
            pr = r;
            ps = s;
        }
        if (diff < 1e-15)
           break;
    }
    p->b.R = pr-1;
    p->b.S = ps-1;
}

const AD9548_Profile_TypeDef *PLL_Prof_filter_1(AD9548_Profile_TypeDef *p)
{
    // filter from ADC64VE all profiles
    p->b.filter_alpha_0_linear = 39705;
    p->b.filter_alpha_1_exp = 5;
    p->b.filter_alpha_2_exp = 0;
    p->b.filter_beta_0_linear = 81944;
    p->b.filter_beta_1_exp = 13;
    p->b.filter_gamma_0_linear = 86346;
    p->b.filter_gamma_1_exp = 13;
    p->b.filter_delta_0_linear = 28989;
    p->b.filter_delta_1_exp = 12;
    p->b.filter_alpha_3_exp = 0;
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_filter_2(AD9548_Profile_TypeDef *p)
{
    // filter from TQDC16VS prof3 41.7 MHz
    // filter from TDC_VHLE prof3 41.7 MHz
    p->b.filter_alpha_0_linear = 39705;
    p->b.filter_alpha_1_exp = 6;
    p->b.filter_alpha_2_exp = 0;
    p->b.filter_beta_0_linear = 81944;
    p->b.filter_beta_1_exp = 13;
    p->b.filter_gamma_0_linear = 86346;
    p->b.filter_gamma_1_exp = 13;
    p->b.filter_delta_0_linear = 28989;
    p->b.filter_delta_1_exp = 12;
    p->b.filter_alpha_3_exp = 0;
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_filter_3(AD9548_Profile_TypeDef *p)
{
    // filter from TQDC16VS prof0 125 MHz
    // filter from TDC_VHLE prof0 125 MHz
    p->b.filter_alpha_0_linear = 52940;
    p->b.filter_alpha_1_exp = 7;
    p->b.filter_alpha_2_exp = 0;
    p->b.filter_beta_0_linear = 81944;
    p->b.filter_beta_1_exp = 13;
    p->b.filter_gamma_0_linear = 86346;
    p->b.filter_gamma_1_exp = 13;
    p->b.filter_delta_0_linear = 28989;
    p->b.filter_delta_1_exp = 12;
    p->b.filter_alpha_3_exp = 0;
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_filter_4(AD9548_Profile_TypeDef *p)
{
    // filter from TQDC16VS prof1 41.7 MHz
    // filter from TDC_VHLE prof1 41.7  MHz
    p->b.filter_alpha_0_linear = 62039;
    p->b.filter_alpha_1_exp = 0;
    p->b.filter_alpha_2_exp = 0;
    p->b.filter_beta_0_linear = 81944;
    p->b.filter_beta_1_exp = 13;
    p->b.filter_gamma_0_linear = 86346;
    p->b.filter_gamma_1_exp = 13;
    p->b.filter_delta_0_linear = 28989;
    p->b.filter_delta_1_exp = 12;
    p->b.filter_alpha_3_exp = 0;
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_filter_5(AD9548_Profile_TypeDef *p)
{
    // filter from TQDC16VS prof2 40 MHz
    // filter from TDC_VHLE prof2 40  MHz
    p->b.filter_alpha_0_linear = 33087;
    p->b.filter_alpha_1_exp = 5;
    p->b.filter_alpha_2_exp = 0;
    p->b.filter_beta_0_linear = 81944;
    p->b.filter_beta_1_exp = 13;
    p->b.filter_gamma_0_linear = 86346;
    p->b.filter_gamma_1_exp = 13;
    p->b.filter_delta_0_linear = 28989;
    p->b.filter_delta_1_exp = 12;
    p->b.filter_alpha_3_exp = 0;
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_default(AD9548_Profile_TypeDef *p)
{
    p->b.phase_lock_scale=0;
    p->b.V=0;
    p->b.U=0;
    p->b.validation_timer=100;
    p->b.redetect_timeout=100;
    p->b.inner_tolerance=50000;
    p->b.outer_tolerance=20000;
    p->b.phase_lock_threshold=100;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=100;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    PLL_Prof_filter_1(p);
    return p;
}

const AD9548_Profile_TypeDef *PLL_Prof_derive_ref_period(AD9548_Profile_TypeDef *p)
{
    p->b.ref_period = ad9548_dds_period_fs * (p->b.S+1) / (p->b.R+1);
    return p;

}
