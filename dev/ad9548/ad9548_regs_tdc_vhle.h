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

#ifndef AD9548_REGS_TDC_VHLE_H
#define AD9548_REGS_TDC_VHLE_H

#include "ad9548_regs_output.h"
#include "ad9548_regs_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void PLL_Output_TDC_VHLE(AD9548_Output_TypeDef *p);
extern void PLL_Prof7_TDC_VHLE(AD9548_Profile_TypeDef *p);
extern void PLL_Prof1_TDC_VHLE(AD9548_Profile_TypeDef *p);
extern void PLL_Prof5_TDC_VHLE(AD9548_Profile_TypeDef *p);
extern void PLL_Prof6_TDC_VHLE(AD9548_Profile_TypeDef *p);
extern void PLL_Prof4_TDC_VHLE(AD9548_Profile_TypeDef *p);

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_TDC_VHLE_H
