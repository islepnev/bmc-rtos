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

#include "devices_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

SensorStatus getMiscStatus(const Devices *d);
SensorStatus pollVxsiicStatus(Devices *dev);
DeviceStatus getDeviceStatus(const Devices *d);
SensorStatus getSystemStatus(void);
encoded_system_status_t encode_system_status(const Devices *dev);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_STATUS_H
