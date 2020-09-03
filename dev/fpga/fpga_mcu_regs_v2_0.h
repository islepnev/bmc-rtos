/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef FPGA_MCU_REGS_V2_0_H
#define FPGA_MCU_REGS_V2_0_H

#include <stdint.h>

#pragma pack(push, 1)

typedef union {
    struct {
    uint8_t unused_1 : 1;
    uint8_t active_ref : 2;
    uint8_t locked : 1;
    uint8_t ref_a_valid: 1;
    uint8_t ref_b_valid: 1;
    uint8_t ref_c_valid: 1;
    uint8_t ref_d_valid: 1;
    uint8_t unused_2: 8;
    } b;
    uint16_t raw;
} fpga_mcu_reg_pll_t;

typedef union {
    struct {
        uint8_t minor : 8;
        uint8_t major : 8;
    } b;
    uint16_t raw;
} bmc_version_t;

// BmcStatus should match SensorStatus
typedef enum {
    BMC_UNKNOWN  = 0,
    BMC_NORMAL   = 1,
    BMC_WARNING  = 2,
    BMC_CRITICAL = 3
} BmcStatus;

typedef union {
    struct {
        BmcStatus status : 2;
        uint16_t unused : 14;
    } b;
    uint16_t raw;
} bmc_status_t;

typedef union {
    struct {
        // 0x00 - 0x07
        uint16_t device_id;
        fpga_mcu_reg_pll_t pll;
        uint16_t unlock_count;
        int16_t temp_1;
        int16_t temp_2;
        int16_t temp_3;
        int16_t temp_4;
        uint16_t unused_07;
        // 0x08 - 0x0F
        bmc_version_t bmc_version;
        uint16_t bmc_revision;
        bmc_status_t bmc_overall_status;
        bmc_status_t bmc_power_status;
        bmc_status_t bmc_pll_status;
        uint16_t live_magic;
        uint16_t live_magic_inverted;
        // 0x10 - 0x1F
        uint16_t supply_voltage_1;
        int16_t supply_current_1;
        uint16_t supply_voltage_2;
        int16_t supply_current_2;
        uint16_t supply_voltage_3;
        int16_t supply_current_3;
        uint16_t supply_voltage_4;
        int16_t supply_current_4;
        uint16_t supply_voltage_5;
        int16_t supply_current_5;
        uint16_t supply_voltage_6;
        int16_t supply_current_6;
        uint16_t unused_1c;
        uint16_t unused_1d;
        uint16_t unused_1e;
        uint16_t unused_1f;
    } b;
    uint16_t raw[32];
} fpga_mcu_regs_t;

#pragma pack(pop)

#endif // FPGA_MCU_REGS_V2_0_H
