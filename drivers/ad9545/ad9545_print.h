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
#ifndef AD9545_PRINT_H
#define AD9545_PRINT_H

#include "ad9545_setup.h"
#include "ad9545_status_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

//void pllPrintRefStatus(const Dev_ad9545 *d, PllRef_TypeDef ref_input);
//void pllPrintDPLLChannelStatus(const Dev_ad9545 *d, PllChannel_TypeDef channel);
void ad9545_verbose_setup(const ad9545_setup_t *setup);
void ad9545_verbose_status(const AD9545_Status *status);
void ad9545_brief_status(const AD9545_Status *status);

#ifdef __cplusplus
}
#endif

#endif // AD9545_PRINT_H
