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
#ifndef AD9545_SETUP_H
#define AD9545_SETUP_H

#include "ad9545_setup_regs.h"

void init_PllSysclkSetup(AD9545_Sysclk_Setup_TypeDef *d);
void init_PllRefSetup(AD9545_Ref_Setup_TypeDef *d);
//void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d);
//void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d);
uint64_t get_dpll_default_ftw(AD9545_Channel_TypeDef channel);
void init_DPLL_Setup(AD9545_DPLL_Setup_TypeDef *d, AD9545_Channel_TypeDef channel);
void init_Pll_OutputDrivers_Setup(AD9545_OutputDrivers_Setup_TypeDef *d);
void init_Pll_DPLLMode_Setup(AD9545_DPLL_Mode_Setup_TypeDef *d);
void init_Pll_OutputDividers_Setup(AD9545_Output_Dividers_Setup_TypeDef *d);

#endif // AD9545_SETUP_H
