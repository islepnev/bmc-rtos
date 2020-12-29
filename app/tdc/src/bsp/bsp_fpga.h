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
0x0040 - 0x005F CSR
0x0100 - 0x01FF Trigger control
0x0200 - 0x02FF TDC Core Registers
0x0400 - 0x04FF WR Time Emulator
0x0700 - 0x07FF Run Statistic Counters
0x0900 - 0x09FF Statistics Readout CSR
0x1000 - 0x10FF Error Counters
*/
enum {
    FPGA_REG_BASE_CSR = 0x0040,
    FPGA_REG_BASE_TRIG_CTRL = 0x0100,
    FPGA_REG_BASE_TDC_CORE = 0x0200,
    FPGA_REG_BASE_WR_EMU = 0x0400,
    FPGA_REG_BASE_RUNSTAT = 0x0700,
    FPGA_REG_BASE_STATS = 0x0900,
    FPGA_REG_BASE_ERRCNT = 0x1000,
    FPGA_REG_BASE_MCU = 0x4000,
};

#ifdef __cplusplus
}
#endif

#endif // BSP_FPGA_H
