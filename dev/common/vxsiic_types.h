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

#ifndef VXSIIC_TYPES_H
#define VXSIIC_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {VXSIIC_SLOTS = 18};

extern uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS];
extern const char *vxsiic_map_slot_to_label[VXSIIC_SLOTS];
extern const uint32_t BMC_MAGIC;

typedef enum {
    // read-only legacy info
    VXSIIC_REG_MAGIC = 0,
    VXSIIC_REG_BMC_VER = 1,
    VXSIIC_REG_MODULE_ID = 2,
    VXSIIC_REG_ENC_STATUS = 3,
    VXSIIC_REG_IIC_OPS = 4,
    VXSIIC_REG_IIC_ERRORS = 5,
    VXSIIC_REG_UPTIME = 6,
    VXSIIC_REG_MODULE_SERIAL = 7,
    VXSIIC_REG_FPGA_FW_VER = 8,

    // writable, since 2.8
    VXSIIC_REG_TTVXS_BMC_VER = 0x100,
    VXSIIC_REG_TTVXS_MODULE_ID = 0x101,
    VXSIIC_REG_TTVXS_MODULE_SERIAL = 0x102,
    VXSIIC_REG_TTVXS_FPGA_FW_VER = 0x103,
    VXSIIC_REG_TTVXS_UPTIME = 0x104,

    IIC_SENSORS_MAP_START = 0x1000, // legacy

} vxsiic_data_reg;

enum {VXSIIC_SCRATCH_MEM_START_ADDR = VXSIIC_REG_TTVXS_BMC_VER};
enum {VXSIIC_SCRATCH_MEM_SIZE = 8};

typedef union {
    struct __attribute__((packed)) {
        uint16_t minor : 8;
        uint16_t major : 8;
        uint16_t patch : 16;
    } b;
    uint32_t raw;
} bmc_ver_t;

typedef bmc_ver_t fw_version_t;

bmc_ver_t make_bmc_ver(uint8_t major, uint8_t minor, uint16_t patch);
fw_version_t make_fw_version(uint16_t fw_ver, uint16_t fw_rev);

typedef struct vxsiic_ttvxs_info {
    bmc_ver_t bmc_ver;
    uint32_t module_id;
    uint32_t module_serial;
    fw_version_t fpga_fw_version;
} vxsiic_ttvxs_info;

#ifdef __cplusplus
}
#endif

#endif // VXSIIC_TYPES_H
