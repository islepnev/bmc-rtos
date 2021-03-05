/*
**    VXS IIC Slave Statistics
**
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef DEV_VXSIICS_STATS_H
#define DEV_VXSIICS_STATS_H

#include <stdint.h>

#include "vxsiic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vxsiic_i2c_stats_t {
    uint32_t ops;
    uint32_t errors;
} vxsiic_i2c_stats_t;

extern vxsiic_i2c_stats_t vxsiic_i2c_stats;
//const struct vxsiic_i2c_stats_t *get_vxsiic_i2c_stats_ptr(void);

extern uint32_t vxsiics_scratch_mem[VXSIIC_SCRATCH_MEM_SIZE];

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIICS_STATS_H
