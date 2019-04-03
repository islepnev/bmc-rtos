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

#ifndef AD9545_UTIL_H
#define AD9545_UTIL_H

#include <stdint.h>

#include "ad9545_setup.h"
#include "dev_pll_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int64_t pll_ftw_rel_ppb(const Dev_pll *d, PllChannel_TypeDef channel);
void pllPrintRefStatusBits(Ref_Status_REG_Type r);
ProfileRefSource_TypeDef pll_get_current_ref(const Dev_pll *d, PllChannel_TypeDef channel);
const char *pllProfileRefSourceStr(ProfileRefSource_TypeDef r);

#ifdef __cplusplus
}
#endif

#endif // AD9545_UTIL_H
