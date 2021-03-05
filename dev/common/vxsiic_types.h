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

enum {VXSIIC_SLOTS = 18};

extern uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS];
extern const char *vxsiic_map_slot_to_label[VXSIIC_SLOTS];
extern const uint32_t BMC_MAGIC;

typedef enum {
    VXSIIC_REG_MAGIC = 0,
    VXSIIC_REG_BMC_VER = 1,
    VXSIIC_REG_MODULE_ID = 2,
    VXSIIC_REG_ENC_STATUS = 3,
    VXSIIC_REG_IIC_OPS = 4,
    VXSIIC_REG_IIC_ERRORS = 5,
    VXSIIC_REG_UPTIME = 6,
    VXSIIC_REG_MODULE_SERIAL = 7,
} vxsiic_data_reg;

#endif // VXSIIC_TYPES_H
