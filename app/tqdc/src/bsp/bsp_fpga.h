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
/*
0x0040 - 0x001F CSR
0x0200 - 0x02FF TTVXS Run Control Core Registers
0x0800 - 0x08FF WhiteRabbit Status
0x0900 - 0x09FF Statistics Readout CSR
0x1000 - 0x10FF HWIP Error Counters
0x1200 - 0x13FF Run Control Counters
0x4000 - 0x40FF MCU SPI Slave
0x7000 - 0x70FF Flash Programmer
*/
enum {
    FPGA_REG_BASE_CSR = 0x0040,
    FPGA_REG_BASE_TTVXS_RC_CORE = 0x0200,
    FPGA_REG_BASE_WR_STATUS = 0x0800,
    FPGA_REG_BASE_TTVXS_STATS = 0x0900,
    FPGA_REG_BASE_HWIP_ERRCNT = 0x1000,
    FPGA_REG_BASE_RC_COUNTERS = 0x1200,
    FPGA_REG_BASE_MCU = 0x4000,
    FPGA_REG_BASE_FLASH_PROG = 0x7000,
};

#ifdef __cplusplus
}
#endif

#endif // BSP_FPGA_H
