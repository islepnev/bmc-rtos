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

#include "ad9548_regs_dpll.h"

const AD9548_Dpll_TypeDef AD9548_Dpll_Default = {
    .b.ftw = 0x224B2646EF7F,
    .b.update_tw = 0,
    .b.pullin_lower_limit = 0,
    .b.pullin_upper_limit = 0xFFFFFF,
    .b.dds_phase_offset_word = 0,
    .b.fixed_phase_lock_offset = 0, // ps
    .b.inc_phase_lock_offset_step_size = 0x3E8, // ps
    .b.phase_slew_limit = 0, // ns/sec
    .b.hist_acc_timer = 500 // 0x7530 // ms
};
