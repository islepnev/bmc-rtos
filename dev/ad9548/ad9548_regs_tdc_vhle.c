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
    p->b.selection_priority = 3;
    p->b.promoted_priority = 3;
    p->b.ref_period = 8000000;
    p->b.R = 19;
    p->b.S = 19;
    p->b.phase_lock_threshold = 10000;
    p->b.freq_lock_threshold = 500;
    ad9548_profile_find_r_s(p, 125.0);
    PLL_Prof_derive_ref_period(p);
    PLL_Prof_filter_3(p);
    return p;
}

// 41.7 MHz
const AD9548_Profile_TypeDef *PLL_Prof1_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    PLL_Prof_default(p);
    p->b.selection_priority = 0;
    p->b.promoted_priority = 0;
    p->b.R = 999;
    p->b.S = 2999;
    PLL_Prof_derive_ref_period(p);
    PLL_Prof_filter_4(p);
    return p;
};

// 40 MHz
const AD9548_Profile_TypeDef *PLL_Prof2_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    PLL_Prof_default(p);
    p->b.selection_priority = 0;
    p->b.promoted_priority = 0;
    p->b.R = 15;
    p->b.S = 49;
    PLL_Prof_derive_ref_period(p);
    PLL_Prof_filter_5(p);
    return p;
}

// 41.7 MHz
const AD9548_Profile_TypeDef *PLL_Prof3_TDC_VHLE(AD9548_Profile_TypeDef *p)
{
    PLL_Prof_default(p);
    p->b.selection_priority = 2;
    p->b.promoted_priority = 2;
    p->b.ref_period = 24000000;
    p->b.R = 9;
    p->b.S = 29;
    PLL_Prof_derive_ref_period(p);
    PLL_Prof_filter_2(p);
    return p;
}
