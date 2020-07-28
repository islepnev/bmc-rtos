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

#include "system_status.h"

#include "app_shared_data.h"
#include "devices_types.h"
#include "dev_thset.h"
#include "system_status_common.h"

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if ((d->sfpiic.dev.device_status == DEVICE_FAIL) ||
        (d->vxsiicm.dev.device_status == DEVICE_FAIL)
        )
        status = DEVICE_FAIL;
    for (int i=0; i<deviceList.count; i++)
        if (deviceList.list[i]->device_status == DEVICE_FAIL)
            status = DEVICE_FAIL;
    return status;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.dev.device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->vxsiicm.dev.device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    return SENSOR_NORMAL;
}

SensorStatus getSystemStatus(void)
{
    const Devices *d = getDevicesConst();
    const SensorStatus powermonStatus = getPowermonStatus(&d->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&d->thset);
    const SensorStatus miscStatus = getMiscStatus(d);
    const SensorStatus fpgaStatus = getFpgaStatus();
    const SensorStatus pllStatus = getPllStatus();
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (powermonStatus > systemStatus)
        systemStatus = powermonStatus;
    if (temperatureStatus > systemStatus)
        systemStatus = temperatureStatus;
    if (miscStatus > systemStatus)
        systemStatus = miscStatus;
    if (fpgaStatus > systemStatus)
        systemStatus = fpgaStatus;
    if (pllStatus > systemStatus)
        systemStatus = pllStatus;
    return systemStatus;
}
