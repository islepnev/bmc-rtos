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

#ifndef AD9548_REGS_PROFILE_H
#define AD9548_REGS_PROFILE_H

#include <stdint.h>

enum { PLL_PROF_SIZE = 50};

#pragma pack(push, 1)

typedef union {
    struct {
        uint8_t selection_priority: 3;
        uint8_t promoted_priority: 3;
        uint8_t unused1: 1;
        uint8_t phase_lock_scale: 1;
        uint64_t ref_period: 50; // fs, up to 1.125 sec
        uint8_t ref_period_padding: 6;
        uint32_t inner_tolerance: 20; // (1/tolerance) 10% down to 1 ppm
        uint8_t inner_tolerance_padding: 4;
        uint32_t outer_tolerance: 20; // (1/tolerance) 10% down to 1 ppm
        uint8_t outer_tolerance_padding: 4;
        uint16_t validation_timer: 16; // ms, up to 65.5 sec
        uint16_t redetect_timeout: 16; // ms, up to 65.5 sec
        uint16_t filter_alpha_0_linear: 16;
        uint8_t filter_alpha_1_exp: 6;
        uint8_t filter_alpha_2_exp: 3;
        uint32_t filter_beta_0_linear: 17;
        uint8_t filter_beta_1_exp: 5;
        uint8_t filter_beta_unused: 1;
        uint32_t filter_gamma_0_linear: 17;
        uint8_t filter_gamma_1_exp: 5;
        uint8_t filter_gamma_unused: 2;
        uint32_t filter_delta_0_linear: 15;
        uint8_t filter_delta_1_exp: 5;
        uint8_t filter_alpha_3_exp: 4;
        uint32_t R : 30;
        uint32_t R_unused : 2;
        uint32_t S : 30;
        uint8_t S_unused : 2;
        uint16_t V : 10;
        uint8_t V_unused : 2;
        uint16_t U : 10;
        uint8_t U_unused : 2;
        uint16_t phase_lock_threshold : 16; // ps
        uint8_t phase_lock_fill_rate : 8;
        uint8_t phase_lock_drain_rate : 8;
        uint32_t freq_lock_threshold : 24; // ps
        uint8_t freq_lock_fill_rate : 8;
        uint8_t freq_lock_drain_rate : 8;
    } b;
    uint8_t v[PLL_PROF_SIZE];
} AD9548_Profile_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_PROFILE_H
