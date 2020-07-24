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

#ifndef DEV_PLL_H
#define DEV_PLL_H

#include <stdbool.h>

#include "ad9545/ad9545_setup_regs.h"
#include "ad9545/ad9545_status_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_ad9545_setup(ad9545_setup_t *setup);
bool ad9545_detect(void);
bool ad9545_setup(const ad9545_setup_t *setup);

void ad9545_reset_i2c(void);
bool ad9545_software_reset(void);
bool ad9545_setup_sysclk(const PllSysclkSetup_TypeDef *sysclkSetup);
bool ad9545_calibrate_sysclk(void);
bool ad9545_read_status(AD9545_Status *status);
bool ad9545_read_sysclk_status(AD9545_Status *status);

void ad9545_reset(void);
bool ad9545_gpio_test(void);
void ad9545_gpio_init(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_PLL_H
