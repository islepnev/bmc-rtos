/*
**    Copyright 2019-2021 Ilja Slepnev
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

#ifndef DEV_VXSIICS_TYPES_H
#define DEV_VXSIICS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "devicebase.h"
#include "ipmi_sensor_types.h"
#include "vxsiic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dev_vxsiics_priv {
    DeviceBase dev;
    uint32_t stateStartTick;
    vxsiic_ttvxs_info ttvxs_info;
    uint32_t ttvxs_info_timestamp;
    uint32_t ttvxs_uptime;
    uint32_t ttvxs_uptime_timestamp;
} Dev_vxsiics_priv;

typedef struct Dev_vxsiics {
    DeviceBase dev;
    Dev_vxsiics_priv priv;
} Dev_vxsiics;

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIICS_TYPES_H
