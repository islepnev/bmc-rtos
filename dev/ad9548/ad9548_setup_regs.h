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

#ifndef AD9548_SETUP_REGS_H
#define AD9548_SETUP_REGS_H

#include "ad9548_regs.h"
#include "ad9548_regs_sysclk.h"
#include "ad9548_regs_refin.h"
#include "ad9548_regs_dpll.h"
#include "ad9548_regs_profile.h"
#include "ad9548_regs_output.h"
#include "ad9548_regs_mfpins.h"
#include "ad9548_regs_irqmask.h"

typedef enum {
    AD9548_REFA = 0,
    AD9548_REFAA = 1,
    AD9548_REFB = 2,
    AD9548_REFBB = 3,
    AD9548_REFC = 4,
    AD9548_REFCC = 5,
    AD9548_REFD = 6,
    AD9548_REFDD = 7
} AD9548_Ref_TypeDef;

enum {AD9548_DPLL_PROFILE_COUNT = 8};

typedef struct ad9548_setup_t {
    AD9548_Sysclk_TypeDef sysclk;
    AD9548_MFPins_TypeDef mfpins;
    AD9548_IRQMask_TypeDef irqmask;
    AD9548_Dpll_TypeDef dpll;
    AD9548_Output_TypeDef output;
    AD9548_RefIn_TypeDef refin;
    AD9548_Profile_TypeDef prof[AD9548_DPLL_PROFILE_COUNT];
} ad9548_setup_t;

#endif // AD9548_SETUP_REGS_H
