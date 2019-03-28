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
#ifndef DEV_TYPES_H
#define DEV_TYPES_H

#include <stdint.h>
#include "ad9545_status.h"
#include "vxsiic_status.h"

typedef enum {
    PGOOD_FAIL = 0,
    PGOOD_OK = 1 }
PgoodState;

typedef enum {
    SENSOR_UNKNOWN  = 0,
    SENSOR_NORMAL   = 1, // numeric value used
    SENSOR_WARNING  = 2,
    SENSOR_CRITICAL = 3
} SensorStatus;

typedef enum {
    DEVICE_UNKNOWN = 0,
    DEVICE_NORMAL = 1,
    DEVICE_FAIL = 2
} DeviceStatus;

enum {DEV_THERM_COUNT = 1};

typedef struct Dev_adt7301 {
    int valid;
    int16_t rawTemp;
} Dev_adt7301;

typedef struct Dev_thset {
    Dev_adt7301 th[DEV_THERM_COUNT];
} Dev_thset;

typedef struct Dev_fpga {
    DeviceStatus present;
    uint8_t id;
} Dev_fpga;

typedef struct Dev_sfpiic {
    DeviceStatus present;
} Dev_sfpiic;

typedef struct Dev_vxsiic {
    DeviceStatus present;
    vxsiic_status_t status;
} Dev_vxsiic;

typedef struct Dev_at24c {
    DeviceStatus present;
} Dev_at24c;

typedef enum {
    PLL_STATE_INIT,
    PLL_STATE_RESET,
    PLL_STATE_SETUP_SYSCLK,
    PLL_STATE_SYSCLK_WAITLOCK,
    PLL_STATE_SETUP,
    PLL_STATE_RUN,
    PLL_STATE_ERROR,
    PLL_STATE_FATAL
} PllState;

typedef struct Dev_ad9545 {
    DeviceStatus present;
    AD9545_Status status;
    PllState fsm_state;
    uint32_t recoveryCount;
} Dev_ad9545;

#endif // DEV_TYPES_H
