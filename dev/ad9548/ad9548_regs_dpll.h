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

#ifndef AD9548_REGS_DPLL_H
#define AD9548_REGS_DPLL_H

#include <stdint.h>

enum { PLL_DPLL_SIZE = 28};

#pragma pack(push, 1)

typedef union {
    struct {
        uint64_t ftw : 48;
        uint8_t update_tw: 1;
        uint8_t update_tw_padding: 7;
        uint32_t pullin_lower_limit : 24;
        uint32_t pullin_upper_limit : 24;
        uint16_t dds_phase_offset_word : 16;
        int64_t fixed_phase_lock_offset : 40; // ps
        uint16_t inc_phase_lock_offset_step_size: 16; // ps
        uint16_t phase_slew_limit: 16; // ns/sec
        uint32_t hist_acc_timer: 24; // ms
        uint8_t hist_inc_average: 3;
        uint8_t hist_persistent: 1;
        uint8_t hist_single_sample_fallback: 1;
        uint8_t hist_unused: 3;
    } b;
    uint8_t v[PLL_DPLL_SIZE];
} AD9548_Dpll_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern const AD9548_Dpll_TypeDef AD9548_Dpll_Default;

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_DPLL_H
