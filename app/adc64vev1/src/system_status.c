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

#include "devicelist.h"
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

encoded_system_status_t encode_system_status(void)
{
    encoded_system_status_t code;
    code.w = 0;
    code.b.system = getSystemStatus() & 0xF;
    code.b.pm =  0;
    code.b.therm = dev_thset_thermStatus() & 0xF;
    code.b.sfpiic = 0;
    code.b.fpga = getFpgaStatus() & 0xF;
    code.b.pll = getPllStatus() & 0xF;
    return code;
}

//SensorStatus get_vxsiicm_sensor_status(const Devices *d)
//{
//    if (d->vxsiicm.dev.device_status != DEVICE_NORMAL)
//        return SENSOR_CRITICAL;
//    return SENSOR_NORMAL;
//}

SensorStatus getSystemStatus(void)
{
    const SensorStatus temperatureStatus = dev_thset_thermStatus();
    const SensorStatus fpgaStatus = getFpgaStatus();
    const SensorStatus pllStatus = getPllStatus();
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (temperatureStatus > systemStatus)
        systemStatus = temperatureStatus;
    if (fpgaStatus > systemStatus)
        systemStatus = fpgaStatus;
    if (pllStatus > systemStatus)
        systemStatus = pllStatus;
    return systemStatus;
}
