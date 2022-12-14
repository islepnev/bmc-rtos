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

#ifndef BSP_FPGA_H
#define BSP_FPGA_H

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern sensor_list_t fpga_sensor_map;

enum {
    FPGA_REG_BASE_CSR = 0x0040,
    FPGA_REG_BASE_MCU = 0x4000,
    FPGA_REG_BASE_FLASH_PROG = 0x7000,
};

#ifdef __cplusplus
}
#endif

#endif // BSP_FPGA_H
