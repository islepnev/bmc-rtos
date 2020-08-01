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

#include "ad9516/dev_auxpll_types.h"
#include "devicelist.h"
#include "powermon/dev_powermon_types.h"
#include "sfpiic/dev_sfpiic.h"
#include "system_status_common.h"
#include "thset/dev_thset.h"

DeviceStatus getDeviceStatus(void)
{
    DeviceStatus status = DEVICE_NORMAL;
    for (int i=0; i<deviceList.count; i++)
        if (deviceList.list[i]->device_status == DEVICE_FAIL)
            status = DEVICE_FAIL;
    return status;
}

//SensorStatus get_vxsiicm_sensor_status(const Devices *d)
//{
//    if (d->vxsiicm.dev.device_status != DEVICE_NORMAL)
//        return SENSOR_CRITICAL;
//    return SENSOR_NORMAL;
//}

SensorStatus getSystemStatus(void)
{
    const SensorStatus powermonStatus = getPowermonStatus();
    const SensorStatus temperatureStatus = dev_thset_thermStatus();
    const SensorStatus sfpiicStatus = get_sfpiic_sensor_status();
    const SensorStatus fpgaStatus = getFpgaStatus();
    const SensorStatus pllStatus = getPllStatus();
    const SensorStatus ad9516Status = get_auxpll_sensor_status();
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (powermonStatus > systemStatus)
        systemStatus = powermonStatus;
    if (temperatureStatus > systemStatus)
        systemStatus = temperatureStatus;
    if (sfpiicStatus > systemStatus)
        systemStatus = sfpiicStatus;
    if (fpgaStatus > systemStatus)
        systemStatus = fpgaStatus;
    if (pllStatus > systemStatus)
        systemStatus = pllStatus;
    if (ad9516Status > systemStatus)
        systemStatus = ad9516Status;
    return systemStatus;
}
