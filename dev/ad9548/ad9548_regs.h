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

enum { PLL_SYSCLK_SIZE	= 9};
enum { PLL_MFPINS_SIZE	= 8};
enum { PLL_IRQ_SIZE	    = 9};
enum { PLL_DPLL_SIZE	= 28};
enum { PLL_OUTCLK_SIZE	= 24};
enum { PLL_REFIN_SIZE	= 8};
enum { PLL_PROF_SIZE	= 50};

typedef struct PLL_Reg
{
    uint16_t address;
    uint8_t data;
    uint8_t __dummy_alignment;
} PLL_Reg;

typedef PLL_Reg PLL_SysClk_regs[PLL_SYSCLK_SIZE];
typedef PLL_Reg PLL_MFPins_regs[PLL_MFPINS_SIZE];
typedef PLL_Reg PLL_IRQ_regs[PLL_IRQ_SIZE];
typedef PLL_Reg PLL_DPLL_regs[PLL_DPLL_SIZE];
typedef PLL_Reg PLL_OutClk_regs[PLL_OUTCLK_SIZE];
typedef PLL_Reg PLL_RefIn_regs[PLL_REFIN_SIZE];
typedef PLL_Reg PLL_Profile_regs[PLL_PROF_SIZE];

extern const PLL_SysClk_regs Default_PLL_SysClk;
extern const PLL_MFPins_regs Default_PLL_MFPins;
extern const PLL_IRQ_regs Default_PLL_IRQ;
extern const PLL_DPLL_regs Default_PLL_DPLL;

extern const PLL_OutClk_regs PLL_OutClk_ADC64VE;
extern const PLL_RefIn_regs PLL_RefIn_ADC64VE ;
extern const PLL_Profile_regs PLL_Prof0_ADC64VE;
extern const PLL_Profile_regs PLL_Prof1_ADC64VE;
extern const PLL_Profile_regs PLL_Prof2_ADC64VE;
extern const PLL_Profile_regs PLL_Prof3_ADC64VE;

extern const PLL_OutClk_regs PLL_OutClk_TDC_VHLE;
extern const PLL_RefIn_regs PLL_RefIn_TDC_VHLE;
extern const PLL_Profile_regs PLL_Prof0_TDC_VHLE;
extern const PLL_Profile_regs PLL_Prof1_TDC_VHLE;
extern const PLL_Profile_regs PLL_Prof2_TDC_VHLE;
extern const PLL_Profile_regs PLL_Prof3_TDC_VHLE;

extern const PLL_OutClk_regs PLL_OutClk_TQDC16VS;
extern const PLL_RefIn_regs PLL_RefIn_TQDC16VS;
extern const PLL_Profile_regs PLL_Prof0_TQDC16VS;
extern const PLL_Profile_regs PLL_Prof1_TQDC16VS;
extern const PLL_Profile_regs PLL_Prof2_TQDC16VS;
extern const PLL_Profile_regs PLL_Prof3_TQDC16VS;

#endif // AD9548_REGS_H
