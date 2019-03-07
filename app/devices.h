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
#include "dev_leds.h"

enum { FPGA_DEVICE_ID = 0xD0};
typedef enum {
    PGOOD_FAIL = 0,
    PGOOD_OK = 1 }
PgoodState;

typedef struct {
    Dev_Leds leds;
    Dev_thset thset;
    Dev_fpga fpga;
    Dev_pca9548 i2cmux;
    Dev_at24c eeprom_vxspb;
    Dev_at24c eeprom_config;
    Dev_ad9545 pll;
    Dev_powermon pm;
} Devices;

void struct_thset_init(Dev_thset *d);
void struct_fpga_init(Dev_fpga *d);
void struct_pca9548_init(Dev_pca9548 *d);
void struct_at24c_init(Dev_at24c *d);
void struct_ad9545_init(Dev_ad9545 *d);
void struct_Devices_init(Devices *d);

void devReset(Devices *d);
DeviceStatus getDeviceStatus(const Devices *d);
DeviceStatus devDetect(Devices *d);
DeviceStatus devRun(Devices *d);
void dev_switchPower(Dev_powermon *pm, SwitchOnOff state);
PgoodState dev_readPgood(Dev_powermon *pm);
void dev_read_thermometers(Devices *dev);
//void dev_print_thermometers(const Devices *dev);

//void dev_thset_read(Dev_thset *d);
//void dev_thset_print(const Dev_thset d);
SensorStatus dev_thset_thermStatus(const Dev_thset *d);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
