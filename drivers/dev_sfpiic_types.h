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

#ifndef DEV_SFPIIC_TYPES_H
#define DEV_SFPIIC_TYPES_H

#include <stdint.h>

#include "devicelist.h"
#include "ipmi_sensor_types.h"

enum {SFPIIC_CH_CNT = 8};
typedef struct sfpiic_stats_t {
    uint32_t ops;
    uint32_t errors;
} sfpiic_stats_t;

typedef enum {
    SFPIIC_CH_STATE_UNKNOWN,
    SFPIIC_CH_STATE_READY,
    SFPIIC_CH_STATE_ERROR
} sfpiic_ch_state_t;

struct sfpiic_ch_status_t {
   int present;
   SensorStatus system_status;
   uint16_t last_iic_addr;
   int16_t temp; // in 1/256 degrees C
   uint16_t volt; // in 100uV
   uint16_t tx_en_cnt;
   char vendor_name[17];
   char vendor_serial[17];
   uint16_t tx_pow[4];
   uint16_t rx_pow[4];
//   GenericSensor sensors[MAX_SENSOR_COUNT];
   sfpiic_stats_t iic_stats;
   sfpiic_stats_t iic_master_stats;
   sfpiic_ch_state_t ch_state;
};

typedef struct sfpiic_ch_status_t sfpiic_ch_status_t;

struct sfpiic_status_t {
   sfpiic_ch_status_t sfp[SFPIIC_CH_CNT];
};

typedef struct sfpiic_status_t sfpiic_status_t;

typedef struct Dev_sfpiic {
    DeviceBase dev;
    sfpiic_status_t status;
} Dev_sfpiic;

#endif // DEV_SFPIIC_TYPES_H
