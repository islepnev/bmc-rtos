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

#ifndef DEVICES_TYPES_H
#define DEVICES_TYPES_H

#include "eeprom_config/dev_eeprom_config.h"
#include "fpga/dev_fpga_types.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux_types.h"
#include "ad9545/dev_ad9545.h"
#include "ad9516/dev_auxpll_types.h"
#include "dev_thset_types.h"
#include "dev_sfpiic_types.h"
#include "vxsiicm/dev_vxsiicm_types.h"
#include "powermon/dev_powermon_types.h"
#include "dev_leds_types.h"

typedef struct Dev_sdcard {
    DeviceBase dev;
    int detect_b;
} Dev_sdcard;

enum {
    PCB_VER_A_MCB_1_0 = 3,
    PCB_VER_A_MCB_1_1 = 1,
};

typedef struct Devices {
    Dev_thset thset;
    Dev_fpga fpga;
    Dev_ttvxs_clkmux clkmux;
    Dev_sfpiic sfpiic;
    Dev_vxsiicm vxsiicm;
    Dev_eeprom_config eeprom_config;
    Dev_auxpll auxpll;
    Dev_powermon pm;
    Dev_sdcard sd;
    int pen_b;
    uint32_t pcb_ver;
} Devices;

#endif // DEVICES_TYPES_H
