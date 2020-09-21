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

#include "ad9548_regs_sysclk.h"

static const double sysclk_osc_MHz = 38.88;
static const int sysclk_n_divider = 24;

const AD9548_Sysclk_TypeDef AD9548_Sysclk_Default = {
    .b.reg_100.b.lockdet_divider = 1,
    .b.reg_100.b.chargepump_mode = 1,
    .b.n_divider = sysclk_n_divider,
    .b.reg_102.b.ref_select = 1,
    .b.reg_102.b.pll_enable = 1,
    .b.reg_102.b.m_divider_reset = 1,
    .b.sysclk_period_fs.b.value = 1000000000. / (sysclk_osc_MHz*sysclk_n_divider), // 1071674
    .b.sysclk_stability_period_ms.b.value = 1
};
