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

#ifndef DEV_AD9545_PRINT_H
#define DEV_AD9545_PRINT_H

#include "dev_common_types.h"

enum { DISPLAY_PLL_H = 5 };
enum { DISPLAY_PLL_DETAIL_H = 31 };

#ifdef __cplusplus
extern "C" {
#endif

void dev_ad9545_verbose_status(void);
void dev_ad9545_print_box(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_AD9545_PRINT_H
