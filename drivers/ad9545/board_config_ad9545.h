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

#ifdef BOARD_CRU16
#define PLL_REF_PERIOD_NS   8
#define PLL_REF_DIV       625
#define PLL_DIST_DIV_0     10
#define PLL_DIST_DIV_1     13

#elif BOARD_TDC64
#define PLL_REF_PERIOD_NS  24
#define PLL_REF_DIV       209
#define PLL_DIST_DIV_0     30
#define PLL_DIST_DIV_1     39

#elif BOARD_TDC72
#define PLL_REF_PERIOD_NS  24
#define PLL_REF_DIV       209
#define PLL_DIST_DIV_0     30
#define PLL_DIST_DIV_1     39

#elif BOARD_TTVXS
#define PLL_REF_PERIOD_NS  24
#define PLL_REF_DIV       209
#define PLL_DIST_DIV_0     30
#define PLL_DIST_DIV_1     39

#else
#error "BOARD variant not defined"
#endif

#endif // BOARD_CONFIG_AD9545_H
