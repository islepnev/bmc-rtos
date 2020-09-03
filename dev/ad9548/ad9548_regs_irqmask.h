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

#ifndef AD9548_REGS_IRQMASK_H
#define AD9548_REGS_IRQMASK_H

#include <stdint.h>

enum { PLL_IRQMASK_SIZE = 8 };

#pragma pack(push, 1)

typedef union {
    struct {
        // reg 0x0D02
        uint8_t sysclk_cal_started: 1;
        uint8_t sysclk_cal_complete: 1;
        uint8_t sysclk_unused1: 2;
        uint8_t sysclk_locked: 1;
        uint8_t sysclk_unlocked: 1;
        uint8_t sysclk_unused2: 2;
        // reg 0x0D03
        uint8_t eeprom_complete: 1;
        uint8_t eeprom_fault: 1;
        uint8_t watchdog_timer: 1;
        uint8_t dist_sync: 1;
        uint8_t unused_0d03: 4;
        // reg 0x0D04
        uint8_t dpll_phase_locked: 1;
        uint8_t dpll_phase_unlocked: 1;
        uint8_t dpll_freq_locked: 1;
        uint8_t dpll_freq_unlocked: 1;
        uint8_t dpll_holdover: 1;
        uint8_t dpll_freerun: 1;
        uint8_t dpll_closed: 1;
        uint8_t dpll_switching: 1;
        // reg 0x0D05
        uint8_t phase_slew_limited: 1;
        uint8_t phase_slew_unlimited: 1;
        uint8_t freq_clamped: 1;
        uint8_t freq_unclamped: 1;
        uint8_t hist_updated: 1;
        uint8_t unused_0d05: 3;
        // reg 0x0D06
        uint8_t ref_a_fault: 1;
        uint8_t ref_a_fault_cleared: 1;
        uint8_t ref_a_validated: 1;
        uint8_t ref_a_new_profile: 1;
        uint8_t ref_aa_fault: 1;
        uint8_t ref_aa_fault_cleared: 1;
        uint8_t ref_aa_validated: 1;
        uint8_t ref_aa_new_profile: 1;
        // reg 0x0D07
        uint8_t ref_b_fault: 1;
        uint8_t ref_b_fault_cleared: 1;
        uint8_t ref_b_validated: 1;
        uint8_t ref_b_new_profile: 1;
        uint8_t ref_bb_fault: 1;
        uint8_t ref_bb_fault_cleared: 1;
        uint8_t ref_bb_validated: 1;
        uint8_t ref_bb_new_profile: 1;
        // reg 0x0D08
        uint8_t ref_c_fault: 1;
        uint8_t ref_c_fault_cleared: 1;
        uint8_t ref_c_validated: 1;
        uint8_t ref_c_new_profile: 1;
        uint8_t ref_cc_fault: 1;
        uint8_t ref_cc_fault_cleared: 1;
        uint8_t ref_cc_validated: 1;
        uint8_t ref_cc_new_profile: 1;
        // reg 0x0D09
        uint8_t ref_d_fault: 1;
        uint8_t ref_d_fault_cleared: 1;
        uint8_t ref_d_validated: 1;
        uint8_t ref_d_new_profile: 1;
        uint8_t ref_dd_fault: 1;
        uint8_t ref_dd_fault_cleared: 1;
        uint8_t ref_dd_validated: 1;
        uint8_t ref_dd_new_profile: 1;
    } b;
    uint8_t v[PLL_IRQMASK_SIZE];
} AD9548_IRQMask_TypeDef;

typedef union {
    struct {
        uint8_t mode: 2;
        uint8_t unused: 6;
    } b;
    uint8_t raw;
} AD9548_IRQPinMode_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern void AD9548_IRQPinMode_Default(AD9548_IRQPinMode_TypeDef *p);
extern void AD9548_IRQMask_Default(AD9548_IRQMask_TypeDef *p);

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_IRQMASK_H
