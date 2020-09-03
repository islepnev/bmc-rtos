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

#ifndef AD9548_REGS_ADC64VE_H
#define AD9548_REGS_ADC64VE_H

#include "ad9548_regs_output.h"
#include "ad9548_regs_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const AD9548_Output_TypeDef PLL_Output_ADC64VE;
const AD9548_Profile_TypeDef *PLL_Prof0_ADC64VE(AD9548_Profile_TypeDef *p);
const AD9548_Profile_TypeDef *PLL_Prof1_ADC64VE(AD9548_Profile_TypeDef *p);
const AD9548_Profile_TypeDef *PLL_Prof2_ADC64VE(AD9548_Profile_TypeDef *p);
const AD9548_Profile_TypeDef *PLL_Prof3_ADC64VE(AD9548_Profile_TypeDef *p);

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_ADC64VE_H
