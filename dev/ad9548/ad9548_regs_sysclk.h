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

#ifndef AD9548_REGS_SYSCLK_H
#define AD9548_REGS_SYSCLK_H

#include <stdint.h>

enum { PLL_SYSCLK_SIZE = 9};

#pragma pack(push, 1)

typedef union {
    struct {
        uint8_t unused:4;
        uint8_t long_instr:1;
        uint8_t softreset:1;
        uint8_t lsb_first:1;
        uint8_t sdo_enable:1;
    } b;
    uint8_t raw;
} AD9548_Control_REG1_Type;

typedef union {
    struct {
        uint8_t lockdet_divider:2;
        uint8_t lockdet_timer_disable:1;
        uint8_t chargepump:3;
        uint8_t chargepump_mode:1;
        uint8_t ext_loopfilter_enable:1;
    } b;
    uint8_t raw;
} AD9548_Sysclk_100_REG1_Type;

typedef union {
    struct {
        uint8_t ref_select:2;
        uint8_t pll_enable:1;
        uint8_t freq_mult_2x_enable:1;
        uint8_t m_divider:2;
        uint8_t m_divider_reset:1;
        uint8_t unused:1;
    } b;
    uint8_t raw;
} AD9548_Sysclk_102_REG1_Type;

typedef union {
    struct {
        uint32_t value:21;
        uint32_t unused:3;
    } b;
    uint8_t raw[3];
} AD9548_Sysclk_103_REG3_Type;

typedef union {
    struct {
        uint32_t value:20;
        uint32_t unused:4;
    } b;
    uint8_t raw[3];
} AD9548_Sysclk_106_REG3_Type;

typedef union {
    struct {
        AD9548_Sysclk_100_REG1_Type reg_100;
        uint8_t n_divider;
        AD9548_Sysclk_102_REG1_Type reg_102;
        AD9548_Sysclk_103_REG3_Type sysclk_period_fs;
        AD9548_Sysclk_106_REG3_Type sysclk_stability_period_ms;
    } b;
    uint8_t v[PLL_SYSCLK_SIZE];
} AD9548_Sysclk_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern const AD9548_Sysclk_TypeDef AD9548_Sysclk_Default;

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_SYSCLK_H
