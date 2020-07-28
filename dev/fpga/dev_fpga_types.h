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

#ifndef DEV_FPGA_TYPES_H
#define DEV_FPGA_TYPES_H

#include <stdint.h>
#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {FPGA_REG_COUNT = 8};
typedef struct Dev_fpga {
    DeviceBase dev;
    uint16_t regs[FPGA_REG_COUNT];
    uint8_t initb;
    uint8_t done;
    uint16_t id;
    uint16_t fw_ver;
    uint16_t fw_rev;
} Dev_fpga;

SensorStatus get_fpga_sensor_status(const Dev_fpga *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_FPGA_TYPES_H
