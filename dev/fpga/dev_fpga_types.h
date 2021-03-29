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

#include "devicebase.h"
#include "sdb.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {FPGA_REG_COUNT = 8};
enum { SDB_MAX_RECORDS = 32 }; // 0x7C00-0x7FFF 16-bit registers capacity
enum { SDB_MAX_DEVICES = SDB_MAX_RECORDS - 1 };

typedef enum {
    FPGA_STATE_STANDBY,
    FPGA_STATE_LOAD,
    FPGA_STATE_RESET,
    FPGA_STATE_RUN,
    FPGA_STATE_PAUSE,
    FPGA_STATE_ERROR
} fpga_state_t;

typedef struct Dev_fpga_priv {
    struct sdb_interconnect sdb_ic;
    struct sdb_device sdb_devices[SDB_MAX_DEVICES];
    uint16_t regs[FPGA_REG_COUNT];
    uint8_t proto_version;
    fpga_state_t state;
    uint32_t fpga_load_start_tick;
    uint32_t stateStartTick;
    uint8_t initb;
    uint8_t done;
    uint16_t id;
    uint16_t id_read;
    uint16_t fw_ver;
    uint16_t fw_rev;
    uint16_t temp;
    uint64_t ow_id;
} Dev_fpga_priv;

typedef struct Dev_fpga {
    DeviceBase dev;
    Dev_fpga_priv priv;
} Dev_fpga;

SensorStatus get_fpga_sensor_status(void);
uint32_t get_fpga_id(void);
uint64_t get_fpga_ow_id(void);
uint32_t get_fpga_serial(void);
uint32_t get_fpga_fw_ver(void);
uint32_t get_fpga_fw_rev(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_FPGA_TYPES_H
