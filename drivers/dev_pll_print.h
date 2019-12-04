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
#ifndef DEV_PLL_PRINT_H
#define DEV_PLL_PRINT_H

#include "dev_pll_types.h"
#include "ad9545_setup.h"
#include "ad9545_status.h"

#ifdef __cplusplus
extern "C" {
#endif

//void pllPrintRefStatus(const Dev_ad9545 *d, PllRef_TypeDef ref_input);
//void pllPrintDPLLChannelStatus(const Dev_ad9545 *d, PllChannel_TypeDef channel);
void pllPrintStatus(const Dev_ad9545 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_PLL_PRINT_H
