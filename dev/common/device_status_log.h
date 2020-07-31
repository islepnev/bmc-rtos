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

#ifndef DEVICE_STATUS_LOG_H
#define DEVICE_STATUS_LOG_H

#include "log/log_prio.h"
#include "dev_common_types.h"
#include "bus/bus_types.h"
#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

LogPriority device_status_log_priority(DeviceStatus status);
void dev_log_status_change(const DeviceBase *dev);
LogPriority sensor_status_log_priority(SensorStatus status);
void sensor_log_status_change(const DeviceBase *p);

#ifdef __cplusplus
}
#endif


#endif // DEVICE_STATUS_LOG_H
