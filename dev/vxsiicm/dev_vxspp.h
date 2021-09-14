/*
**    VXS Payload Port (IIC Slave)
**
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

#ifndef DEV_VXSPP_H
#define DEV_VXSPP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_common_types.h"
#include "devicebase.h"
#include "ipmi_sensor_types.h"
#include "vxsiic_types.h"

//enum {MCU_MAP_SIZE = 16};
enum {MCU_ID_SIZE = 16};

typedef struct iic_stats_t {
    uint32_t ops;
    uint32_t errors;
} iic_stats_t;

enum {VXSIIC_PP_IOEXP_BIT_RESET = 0x1};
enum {VXSIIC_PP_IOEXP_BIT_DONE  = 0x2};
enum {VXSIIC_PP_IOEXP_BIT_INITB = 0x4};
enum {VXSIIC_PP_IOEXP_BIT_PGOOD = 0x8};

typedef struct vxsiic_pp_control_t {
    bool reset;
} vxsiic_pp_control_t;

typedef struct vxsiic_pp_state_t {
    bool eeprom_found;
    bool gpio_found;
    bool mcu_found;
    bool mcu_info_ok;
    bool mcu_sensors_ok;
} vxsiic_pp_state_t;

typedef struct vxsiic_pp_mcu_info {
    uint32_t magic;
    uint32_t uptime;
    uint32_t module_serial;
    fw_version_t fpga_fw_ver;
    bmc_ver_t bmc_ver;
    uint32_t module_id;
    iic_stats_t iic_stats;
    encoded_system_status_t enc_status;
} vxsiic_pp_mcu_info;

typedef struct vxsiic_pp_mcu_sensors {
    uint16_t count;
    GenericSensor sensors[MAX_SENSOR_COUNT];
} vxsiic_pp_mcu_sensors;

typedef struct Dev_vxspp_priv {
    bool bus_ready;
    int present;
   SensorStatus system_status;
   uint32_t ioexp;
   char module_id_str[MCU_ID_SIZE];
//   uint32_t map[MCU_MAP_SIZE];
   vxsiic_pp_mcu_info mcu_info;
   vxsiic_pp_mcu_sensors mcu_sensors;
   iic_stats_t iic_master_stats;
   vxsiic_pp_state_t pp_state;
} Dev_vxspp_priv;

typedef struct Dev_vxspp {
    DeviceBase dev;
    Dev_vxspp_priv priv;
} Dev_vxspp;

DeviceStatus dev_vxspp_run(struct Dev_vxspp *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSPP_H
