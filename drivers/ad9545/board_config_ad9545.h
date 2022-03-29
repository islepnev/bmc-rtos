/*
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef BOARD_CONFIG_AD9545_H
#define BOARD_CONFIG_AD9545_H

#include "bsp.h"

// OUT Frequency = APLL_Frequency / 2 / PLL_DIST_DIV
// APLL PFD input = APLL_Frequency / APLL_M_Divider

#if defined (BOARD_CRU16_TEST_100)
#define DPLL0_BW_HZ          2000
#define DPLL1_BW_HZ          2000
#define PLL_REFA_PERIOD_NS   8
#define PLL_REFB_PERIOD_NS   8
#define PLL_REFA_DIV         625
#define PLL_REFB_DIV         625
#define DPLL0_EXT_ZD         0
#define DPLL0_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT0C
#define APLL0_Frequency      3.0e9
#define APLL0_M_Divider      9
#define PLL_DIST_DIV_0_A     15
#define PLL_DIST_DIV_0_B     15
#define PLL_DIST_DIV_0_C     12
#define DPLL1_EXT_ZD         0
#define DPLL1_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT1A
#define APLL1_Frequency      3.25e9
#define APLL1_M_Divider      10
#define PLL_DIST_DIV_1_A     13
#define PLL_DIST_DIV_1_B     13

#elif defined (BOARD_ADC64VE)

#define DPLL0_BW_HZ          2000
#define DPLL1_BW_HZ          2000
#define PLL_REFA_PERIOD_NS   8
#define PLL_REFB_PERIOD_NS   8
#define PLL_REFA_DIV         625
#define PLL_REFB_DIV         625
#define DPLL0_EXT_ZD         1
#define DPLL0_ZD_FB_Path     PROFILE_EXT_ZD_FEEDBACK_REFB
#define APLL0_Frequency      2.5e9
#define APLL0_M_Divider      8
#define PLL_DIST_DIV_0_A     10
#define PLL_DIST_DIV_0_B     20
#define PLL_DIST_DIV_0_C     20
#define DPLL1_EXT_ZD         0
#define DPLL1_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT1B
#define APLL1_Frequency      3.75e9
#define APLL1_M_Divider      11
#define PLL_DIST_DIV_1_A     12
#define PLL_DIST_DIV_1_B     15

#elif defined (BOARD_CRU16)

#define DPLL0_BW_HZ          2000
#define DPLL1_BW_HZ          2000
#define PLL_REFA_PERIOD_NS   8
#define PLL_REFB_PERIOD_NS   8
#define PLL_REFA_DIV         625
#define PLL_REFB_DIV         625
#define DPLL0_EXT_ZD         1
#define DPLL0_ZD_FB_Path     PROFILE_EXT_ZD_FEEDBACK_REFB
#define APLL0_Frequency      2.5e9
#define APLL0_M_Divider      8
#define PLL_DIST_DIV_0_A     10
#define PLL_DIST_DIV_0_B     10
#define PLL_DIST_DIV_0_C     10
#define DPLL1_EXT_ZD         0
#define DPLL1_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT1A
#define APLL1_Frequency      3.25e9
#define APLL1_M_Divider      10
#define PLL_DIST_DIV_1_A     13
#define PLL_DIST_DIV_1_B     13

#elif (defined (BOARD_TDC64VLE) || defined (BOARD_TQDC))

#define DPLL0_BW_HZ          500
#define DPLL1_BW_HZ          500
#define PLL_REFA_PERIOD_NS   24
#define PLL_REFB_PERIOD_NS   24
#define PLL_REFA_DIV         209
#define PLL_REFB_DIV         209
#define DPLL0_EXT_ZD         1
#define DPLL0_ZD_FB_Path     PROFILE_EXT_ZD_FEEDBACK_REFB
#define APLL0_Frequency      2.5e9
#define APLL0_M_Divider      8
#define PLL_DIST_DIV_0_A     30
#define PLL_DIST_DIV_0_B     10
#define PLL_DIST_DIV_0_C     30
#define DPLL1_EXT_ZD         0
#define DPLL1_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT1A
#define APLL1_Frequency      3.75e9
#define APLL1_M_Divider      11
#define PLL_DIST_DIV_1_A     45
#define PLL_DIST_DIV_1_B     12
#else

#define DPLL0_BW_HZ          500
#define DPLL1_BW_HZ          500
#define PLL_REFA_PERIOD_NS   24
#define PLL_REFB_PERIOD_NS   24
#define PLL_REFA_DIV         209
#define PLL_REFB_DIV         209
#define DPLL0_EXT_ZD         1
#define DPLL0_ZD_FB_Path     PROFILE_EXT_ZD_FEEDBACK_REFB
#define APLL0_Frequency      2.5e9
#define APLL0_M_Divider      8
#define PLL_DIST_DIV_0_A     30
#define PLL_DIST_DIV_0_B     10
#define PLL_DIST_DIV_0_C     30
#define DPLL1_EXT_ZD         0
#define DPLL1_ZD_FB_Path     PROFILE_INT_ZD_FEEDBACK_OUT1A
#define APLL1_Frequency      3.25e9
#define APLL1_M_Divider      10
#define PLL_DIST_DIV_1_A     39
#define PLL_DIST_DIV_1_B     39

#endif

#endif // BOARD_CONFIG_AD9545_H
