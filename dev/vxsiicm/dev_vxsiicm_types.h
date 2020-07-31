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

#ifndef DEV_VXSIICM_TYPES_H
#define DEV_VXSIICM_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "devicelist.h"
#include "ipmi_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {VXSIIC_SLOTS = 18};
//enum {MCU_MAP_SIZE = 16};
enum {MCU_ID_SIZE = 16};

extern const uint32_t BMC_MAGIC;

typedef struct iic_stats_t {
    uint32_t ops;
    uint32_t errors;
} iic_stats_t;

enum {VXSIIC_PP_IOEXP_BIT_RESET = 0x1};
enum {VXSIIC_PP_IOEXP_BIT_DONE  = 0x2};
enum {VXSIIC_PP_IOEXP_BIT_INITB = 0x4};
enum {VXSIIC_PP_IOEXP_BIT_PGOOD = 0x8};

//typedef enum {
//    VXSIIC_PP_STATE_UNKNOWN,
//    VXSIIC_PP_STATE_READY,
//    VXSIIC_PP_STATE_ERROR
//} vxsiic_pp_state_t;

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
    uint32_t bmc_ver;
    uint32_t module_id;
    iic_stats_t iic_stats;
    encoded_system_status_t enc_status;
} vxsiic_pp_mcu_info;

typedef struct vxsiic_pp_mcu_sensors {
    uint16_t count;
    GenericSensor sensors[MAX_SENSOR_COUNT];
} vxsiic_pp_mcu_sensors;

typedef struct vxsiic_slot_status_t {
   int present;
   SensorStatus system_status;
   uint32_t ioexp;
   char module_id_str[MCU_ID_SIZE];
//   uint32_t map[MCU_MAP_SIZE];
   vxsiic_pp_mcu_info mcu_info;
   vxsiic_pp_mcu_sensors mcu_sensors;
   iic_stats_t iic_master_stats;
   vxsiic_pp_state_t pp_state;
} vxsiic_slot_status_t;

typedef struct vxsiic_status_t {
   vxsiic_slot_status_t slot[VXSIIC_SLOTS];
} vxsiic_status_t;

typedef struct Dev_vxsiicm_priv {
    DeviceBase dev;
    vxsiic_status_t status;
} Dev_vxsiicm_priv;

typedef struct Dev_vxsiicm {
    DeviceBase dev;
    Dev_vxsiicm_priv priv;
} Dev_vxsiicm;

void struct_vxs_i2c_init(Dev_vxsiicm *d);

//typedef struct vxsiic_i2c_board_stats_t {
//    uint32_t ops;
//    uint32_t errors;
//} vxsiic_i2c_board_stats_t;

//typedef struct vxsiic_i2c_stats_t {
//    vxsiic_i2c_board_stats_t pp[VXSIIC_SLOTS];
//} vxsiic_i2c_stats_t;

extern uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS];
extern const char *vxsiic_map_slot_to_label[VXSIIC_SLOTS];
uint8_t get_vxsiic_board_count(const Dev_vxsiicm_priv *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIICM_TYPES_H
