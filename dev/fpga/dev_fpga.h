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

#ifndef DEV_FPGA_H
#define DEV_FPGA_H

#include <stdbool.h>
#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_fpga;
struct Devices;

bool fpgaDetect(struct Dev_fpga *d);
bool fpga_check_live_magic(void);
bool fpga_test(void);
bool fpgaWriteBmcVersion(void);
bool fpgaWriteBmcTemperature(void);
bool fpgaWritePllStatus(void);
bool fpgaWriteSystemStatus(const struct Devices *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_FPGA_H
