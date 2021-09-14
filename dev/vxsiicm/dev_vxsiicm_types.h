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

#include "devicebase.h"
#include "dev_vxspp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dev_vxsiicm_priv {
    DeviceBase dev;
    Dev_vxspp vxspp[VXSIIC_SLOTS];
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

uint8_t get_vxsiicm_board_count(const Dev_vxsiicm_priv *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIICM_TYPES_H
