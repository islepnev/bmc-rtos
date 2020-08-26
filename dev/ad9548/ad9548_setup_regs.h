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

enum { PLL_MFPINS_SIZE	= 8};
enum { PLL_IRQ_SIZE	    = 9};
enum { PLL_OUTCLK_SIZE	= 24};

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


//typedef PLL_Reg AD9548_SysClk_regs[PLL_SYSCLK_SIZE];
typedef PLL_Reg AD9548_MFPins_regs[PLL_MFPINS_SIZE];
typedef PLL_Reg AD9548_IRQ_regs[PLL_IRQ_SIZE];
//typedef PLL_Reg AD9548_DPLL_regs[PLL_DPLL_SIZE];
typedef PLL_Reg AD9548_OutClk_regs[PLL_OUTCLK_SIZE];
//typedef PLL_Reg AD9548_RefIn_regs[PLL_REFIN_SIZE];
typedef PLL_Reg AD9548_Profile_regs[PLL_PROF_SIZE];

//extern const AD9548_SysClk_regs Default_PLL_SysClk;

extern const AD9548_MFPins_regs Default_PLL_MFPins;
extern const AD9548_IRQ_regs Default_PLL_IRQ;
//extern const AD9548_DPLL_regs Default_PLL_DPLL;

extern const AD9548_OutClk_regs PLL_OutClk_ADC64VE;
//extern const AD9548_RefIn_regs PLL_RefIn_ADC64VE ;
extern const AD9548_Profile_regs PLL_Prof0_ADC64VE;
extern const AD9548_Profile_regs PLL_Prof1_ADC64VE;
extern const AD9548_Profile_regs PLL_Prof2_ADC64VE;
extern const AD9548_Profile_regs PLL_Prof3_ADC64VE;

extern const AD9548_OutClk_regs PLL_OutClk_TDC_VHLE;
//extern const AD9548_RefIn_regs PLL_RefIn_TDC_VHLE;
extern const AD9548_Profile_regs PLL_Prof0_TDC_VHLE;
extern const AD9548_Profile_regs PLL_Prof1_TDC_VHLE;
extern const AD9548_Profile_regs PLL_Prof2_TDC_VHLE;
extern const AD9548_Profile_regs PLL_Prof3_TDC_VHLE;

extern const AD9548_OutClk_regs PLL_OutClk_TQDC16VS;
//extern const AD9548_RefIn_regs PLL_RefIn_TQDC16VS;
extern const AD9548_Profile_regs PLL_Prof0_TQDC16VS;
extern const AD9548_Profile_regs PLL_Prof1_TQDC16VS;
extern const AD9548_Profile_regs PLL_Prof2_TQDC16VS;
extern const AD9548_Profile_regs PLL_Prof3_TQDC16VS;

typedef struct ad9548_setup_t {
    AD9548_Sysclk_TypeDef sysclk;//PLL_SysClk;
    AD9548_MFPins_regs PLL_MFPins;
    AD9548_IRQ_regs PLL_IRQ;
    AD9548_Dpll_TypeDef dpll;
    AD9548_OutClk_regs PLL_OutClk;
    AD9548_RefIn_TypeDef refin;// PLL_RefIn;
    AD9548_Profile_regs PLL_Prof0;
    AD9548_Profile_regs PLL_Prof1;
    AD9548_Profile_regs PLL_Prof2;
    AD9548_Profile_regs PLL_Prof3;
} ad9548_setup_t;

#endif // AD9548_SETUP_REGS_H
