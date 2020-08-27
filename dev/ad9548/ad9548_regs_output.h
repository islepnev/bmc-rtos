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

#ifndef AD9548_REGS_OUTPUT_H
#define AD9548_REGS_OUTPUT_H

#include <stdint.h>

enum { PLL_OUTCLK_SIZE	= 24};

#pragma pack(push, 1)
typedef union {
    struct {
        uint8_t powerdown_0:1;
        uint8_t powerdown_1:1;
        uint8_t powerdown_2:1;
        uint8_t powerdown_3:1;
        uint8_t receiver_mode:1;
        uint8_t ext_distrib_resistor:1;
        uint8_t unused_400:2;
        uint8_t en_0:1;
        uint8_t en_1:1;
        uint8_t en_2:1;
        uint8_t en_3:1;
        uint8_t unused_401:4;
        uint8_t sync_mask_0:1;
        uint8_t sync_mask_1:1;
        uint8_t sync_mask_2:1;
        uint8_t sync_mask_3:1;
        uint8_t sync_source:2;
        uint8_t unused_402:2;
        uint8_t autosync_mode:2;
        uint8_t unused_403:6;
        uint8_t out_0_mode:3;
        uint8_t out_0_drive:1;
        uint8_t out_0_invert:1;
        uint8_t out_0_cmos_invert:1;
        uint8_t out_0_unused:2;
        uint8_t out_1_mode:3;
        uint8_t out_1_drive:1;
        uint8_t out_1_invert:1;
        uint8_t out_1_cmos_invert:1;
        uint8_t out_1_unused:2;
        uint8_t out_2_mode:3;
        uint8_t out_2_drive:1;
        uint8_t out_2_invert:1;
        uint8_t out_2_cmos_invert:1;
        uint8_t out_2_unused:2;
        uint8_t out_3_mode:3;
        uint8_t out_3_drive:1;
        uint8_t out_3_invert:1;
        uint8_t out_3_cmos_invert:1;
        uint8_t out_3_unused:2;
        uint32_t q_0:30;
        uint32_t q_0_padding:2;
        uint32_t q_1:30;
        uint32_t q_1_padding:2;
        uint32_t q_2:30;
        uint32_t q_2_padding:2;
        uint32_t q_3:30;
        uint32_t q_3_padding:2;
    } b;
    uint8_t v[PLL_OUTCLK_SIZE];
} AD9548_Output_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_OUTPUT_H
