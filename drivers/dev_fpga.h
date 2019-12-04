//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef DEV_FPGA_H
#define DEV_FPGA_H

#include <unistd.h>
#include "stm32f7xx_hal_def.h"
#include "dev_fpga_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_fpga;
struct Dev_thset;
struct Dev_ad9545;
struct Devices;
struct Dev_powermon;

DeviceStatus fpgaDetect(struct Dev_fpga *d);
DeviceStatus fpga_check_live_magic(Dev_fpga *d);
DeviceStatus fpga_test(struct Dev_fpga *d);
HAL_StatusTypeDef fpgaWriteBmcVersion(void);
HAL_StatusTypeDef fpgaWriteBmcTemperature(const struct Dev_thset *thset);
HAL_StatusTypeDef fpgaWritePllStatus(const struct Dev_ad9545 *pll);
HAL_StatusTypeDef fpgaWriteSystemStatus(const struct Devices *d);
HAL_StatusTypeDef fpgaWriteSensors(const struct Dev_powermon *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_FPGA_H
