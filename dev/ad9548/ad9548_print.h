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
#ifndef AD9548_PRINT_H
#define AD9548_PRINT_H

#include "ad9548_setup.h"
#include "ad9548_setup_regs.h"
#include "ad9548_status_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

//void pllPrintRefStatus(const Dev_ad9548 *d, PllRef_TypeDef ref_input);
//void pllPrintDPLLChannelStatus(const Dev_ad9548 *d, PllChannel_TypeDef channel);
void ad9548_verbose_setup(const ad9548_setup_t *setup);
void ad9548_verbose_status(const ad9548_setup_t *setup, const AD9548_Status *status);
void ad9548_brief_status(const ad9548_setup_t *setup, const AD9548_Status *status);
void ad9548_print_irq_status(const AD9548_IRQMask_TypeDef *irq);

#ifdef __cplusplus
}
#endif

#endif // AD9548_PRINT_H
