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

#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Devices;
struct Dev_fpga;
struct Dev_ad9545;

typedef union
{
  struct
  {
      SensorStatus system:4;
      SensorStatus pm:4;
      SensorStatus therm:4;
      SensorStatus misc:4;
      SensorStatus fpga:4;
      SensorStatus pll:4;
      SensorStatus r1:4;
      SensorStatus r2:4;
  } b;
  uint32_t w;
} encoded_system_status_t;

DeviceStatus getDeviceStatus(const struct Devices *d);
SensorStatus getSystemStatus(void);
SensorStatus getFpgaStatus(const struct Dev_fpga *d);
SensorStatus getPllStatus(const struct Dev_ad9545 *d);
bool getPllLockState(const struct Dev_ad9545 *d);

encoded_system_status_t encode_system_status(const struct Devices *dev);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_STATUS_H
