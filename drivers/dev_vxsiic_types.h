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
#ifndef DEV_VXSIIC_TYPES_H
#define DEV_VXSIIC_TYPES_H

#include <stdint.h>

#include "dev_common_types.h"
#include "ipmi_sensor_types.h"

enum {VXSIIC_SLOTS = 18};
//enum {MCU_MAP_SIZE = 16};
enum {MCU_ID_SIZE = 16};

const uint32_t BMC_MAGIC;

typedef struct iic_stats_t {
    uint32_t ops;
    uint32_t errors;
} iic_stats_t;

enum {VXSIIC_PP_IOEXP_BIT_RESET = 0x1};
enum {VXSIIC_PP_IOEXP_BIT_DONE  = 0x2};
enum {VXSIIC_PP_IOEXP_BIT_INITB = 0x4};
enum {VXSIIC_PP_IOEXP_BIT_PGOOD = 0x8};

typedef union
{
  struct
  {
      SensorStatus system:4;
      SensorStatus pm:4;
      SensorStatus therm:4;
      SensorStatus misc:4;
      SensorStatus fpga:4;
      SensorStatus pll:4;
      SensorStatus r1:4;
      SensorStatus r2:4;
  } b;
  uint32_t w;
} encoded_system_status_t;

typedef enum {
    VXSIIC_PP_STATE_UNKNOWN,
    VXSIIC_PP_STATE_READY,
    VXSIIC_PP_STATE_ERROR
} vxsiic_pp_state_t;

struct vxsiic_slot_status_t {
   int present;
   encoded_system_status_t enc_status;
   DeviceStatus device_status;
   uint32_t ioexp;
   uint32_t magic;
   uint32_t bmc_ver;
   uint32_t module_id;
   uint32_t uptime;
   char module_id_str[MCU_ID_SIZE];
//   uint32_t map[MCU_MAP_SIZE];
   uint16_t sensor_count;
   GenericSensor sensors[MAX_SENSOR_COUNT];
   iic_stats_t iic_stats;
   iic_stats_t iic_master_stats;
   vxsiic_pp_state_t pp_state;
};

typedef struct vxsiic_slot_status_t vxsiic_slot_status_t;

struct vxsiic_status_t {
   vxsiic_slot_status_t slot[VXSIIC_SLOTS];
};

typedef struct vxsiic_status_t vxsiic_status_t;

typedef struct Dev_vxsiic {
    DeviceStatus present;
    vxsiic_status_t status;
} Dev_vxsiic;

//typedef struct vxsiic_i2c_board_stats_t {
//    uint32_t ops;
//    uint32_t errors;
//} vxsiic_i2c_board_stats_t;

//typedef struct vxsiic_i2c_stats_t {
//    vxsiic_i2c_board_stats_t pp[VXSIIC_SLOTS];
//} vxsiic_i2c_stats_t;

extern uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS];
extern const char *vxsiic_map_slot_to_label[VXSIIC_SLOTS];
uint8_t get_vxsiic_board_count(const Dev_vxsiic *d);

#endif // DEV_VXSIIC_TYPES_H
