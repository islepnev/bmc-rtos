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

#ifndef AD9548_REGS_H
#define AD9548_REGS_H

#include <stdint.h>

typedef struct PLL_Reg
{
    uint16_t address;
    uint8_t data;
    uint8_t __dummy_alignment;
} PLL_Reg;

enum {
    AD9545_REG1_CONFIG = 0x0000,
    AD9545_REG1_REVISION_ID = 0x0002,
    AD9545_REG1_DEVICE_ID = 0x0003,

    AD9545_REG_SYSCLK_BASE = 0x0100,

    AD9545_REG_GENERAL_CONFIG_BASE = 0x0200,

    AD9545_REG_DPLL_BASE = 0x0300,

    AD9545_REG_OUTPUT_BASE = 0x0400,

    AD9545_REG_REFIN_BASE = 0x0500,

    AD9545_REG_PROFILE_0_BASE = 0x0600,
    AD9545_REG_PROFILE_1_BASE = 0x0632,
    AD9545_REG_PROFILE_2_BASE = 0x0680,
    AD9545_REG_PROFILE_3_BASE = 0x06B2,
    AD9545_REG_PROFILE_4_BASE = 0x0700,
    AD9545_REG_PROFILE_5_BASE = 0x0732,
    AD9545_REG_PROFILE_6_BASE = 0x0780,
    AD9545_REG_PROFILE_7_BASE = 0x07B2,

    AD9545_REG_OPCONTROL_BASE = 0x0A00,

    AD9545_REG_SCRATCH_BASE = 0x0C00,

    AD9545_REG_STATUS_BASE = 0x0D00,

    AD9545_REG_EEPROMCTRL_BASE = 0x0E00
};

#endif // AD9548_REGS_H
