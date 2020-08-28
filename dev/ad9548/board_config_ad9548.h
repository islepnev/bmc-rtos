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

#ifndef BOARD_CONFIG_AD9548_H
#define BOARD_CONFIG_AD9548_H

#include "bsp.h"

#include "ad9548_regs_adc64ve.h"
#include "ad9548_regs_tdc_vhle.h"
#include "ad9548_regs_tqdc16vs.h"

#if defined (BOARD_TDC72VHLV3)
#elif defined (BOARD_TDC72VHLV2)
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const uint64_t ad9548_dds_period_fs;

extern const char *ad9548_ref_label(int refIndex);

#ifdef __cplusplus
}
#endif

#endif // BOARD_CONFIG_AD9548_H
