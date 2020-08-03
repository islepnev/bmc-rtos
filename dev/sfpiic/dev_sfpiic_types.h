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

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"
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

typedef enum {
    SFPIIC_STATE_SHUTDOWN,
    SFPIIC_STATE_RESET,
    SFPIIC_STATE_RUN,
    SFPIIC_STATE_PAUSE,
    SFPIIC_STATE_ERROR
} sfpiic_state_t;

typedef unsigned char idprom_t[256];

struct sfpiic_ch_status_t {
   int present;
   bool supported;
   bool dom_supported;
   bool is_cxp;
   bool is_sfp;
   bool is_qsfp;
   uint32_t id_updated_timetick;
   uint32_t dom_updated_timetick;
   SensorStatus system_status;
   uint16_t last_iic_addr;
   uint16_t tx_en_cnt;
   uint8_t transceiver;
   uint8_t connector;
   uint8_t compliance_ethernet_1g;
   uint8_t compliance_ethernet_10_40_100;
   uint8_t compliance_extended;
   char vendor_name[17];
   char vendor_pn[17];
   char vendor_serial[17];
   double rx_power_mw[4];
   double tx_power_mw[4];
   double bias_cur[4];
   double voltage;
   double temp;
   idprom_t idprom;
   idprom_t diag;
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

enum { SFPIIC_PORT_NAME_LEN = 16 };
typedef struct Dev_sfpiic_priv {
    sfpiic_state_t state;
    uint32_t stateStartTick;
    sfpiic_status_t status;
    uint32_t portCount;
    uint32_t portIndex[SFPIIC_CH_CNT];
    char portName[SFPIIC_CH_CNT][SFPIIC_PORT_NAME_LEN+1];
    bool multilane[SFPIIC_CH_CNT];
} Dev_sfpiic_priv;

typedef struct Dev_sfpiic {
    DeviceBase dev;
    Dev_sfpiic_priv priv;
} Dev_sfpiic;

#endif // DEV_SFPIIC_TYPES_H
