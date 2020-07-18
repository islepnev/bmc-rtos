/*
**    Copyright 2019 Ilja Slepnev
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
#ifndef BSP_AD9545_H
#define BSP_AD9545_H

#define PLL_REF_PERIOD_NS_TTVXS 24
#define PLL_REF_PERIOD_NS_CRU16 8
#define PLL_REF_PERIOD_NS PLL_REF_PERIOD_NS_CRU16

#define PLL_REF_DIV_TTVXS 209
#define PLL_REF_DIV_CRU16 625
#define PLL_REF_DIV PLL_REF_DIV_CRU16

#include "ad9545/ad9545_setup_regs.h"

void init_PllSysclkSetup(PllSysclkSetup_TypeDef *d);
void init_PllRefSetup(PllRefSetup_TypeDef *d);
//void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d);
//void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d);
uint64_t get_dpll_default_ftw(PllChannel_TypeDef channel);
ProfileRefSource_TypeDef get_dpll_default_ref_source(PllChannel_TypeDef channel);
void init_DPLL_Setup(Pll_DPLL_Setup_TypeDef *d, PllChannel_TypeDef channel);
void init_Pll_OutputDrivers_Setup(Pll_OutputDrivers_Setup_TypeDef *d);
void init_Pll_DPLLMode_Setup(Pll_DPLLMode_Setup_TypeDef *d);
void init_Pll_OutputDividers_Setup(Pll_OutputDividers_Setup_TypeDef *d);

#endif // BSP_AD9545_H
