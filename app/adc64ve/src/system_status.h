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

#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

DeviceStatus getDeviceStatus(void);
SensorStatus getSystemStatus(void);

encoded_system_status_t encode_system_status(void);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_STATUS_H
