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

#ifndef DEVICES_H
#define DEVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "dev_types.h"
#include "dev_powermon.h"
#include "dev_thset.h"
#include "dev_leds.h"

enum { FPGA_DEVICE_ID = 0x2FD0};

typedef struct Devices {
    Dev_Leds leds;
    Dev_thset thset;
    Dev_fpga fpga;
    Dev_pca9548 i2cmux;
    Dev_at24c eeprom_vxspb;
    Dev_at24c eeprom_config;
    Dev_ad9545 pll;
    Dev_powermon pm;
} Devices;

void struct_pca9548_init(Dev_pca9548 *d);
void struct_at24c_init(Dev_at24c *d);
//void struct_ad9545_init(Dev_ad9545 *d);
void struct_Devices_init(Devices *d);

DeviceStatus getDeviceStatus(const Devices *d);
DeviceStatus devDetect(Devices *d);
DeviceStatus devRun(Devices *d);
PgoodState dev_readPgood(Dev_powermon *pm);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
