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
#include "app_task_powermon_impl.h"
#include "thset/dev_thset.h"
#include "digipot/dev_digipot_types.h"
#include "fpga/dev_fpga.h"
#include "system_status_common.h"
#include "powermon/dev_powermon.h"
#include "ad9516/dev_auxpll_types.h"
#include "vxsiics/dev_vxsiics_stats.h"

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if ((d->sfpiic.dev.device_status == DEVICE_FAIL)
        )
        status = DEVICE_FAIL;
    for (int i=0; i<deviceList.count; i++)
        if (deviceList.list[i]->device_status == DEVICE_FAIL)
            status = DEVICE_FAIL;
    return status;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.dev.device_status == DEVICE_FAIL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus pollVxsiicStatus(Devices *dev)
{
    static vxsiic_i2c_stats_t vxsiic_i2c_stats_save = {0};
//    const vxsiic_i2c_stats_t * vxsiic_i2c_stats = get_vxsiic_i2c_stats_ptr();
    uint32_t vxsiic_errors = vxsiic_i2c_stats.errors - vxsiic_i2c_stats_save.errors;
    const SensorStatus vxsiicStatus = vxsiic_errors ? SENSOR_CRITICAL : (vxsiic_i2c_stats.errors) ? SENSOR_WARNING : SENSOR_NORMAL;
    vxsiic_i2c_stats_save = vxsiic_i2c_stats;
    return vxsiicStatus;
}

encoded_system_status_t encode_system_status(const Devices *dev)
{
    encoded_system_status_t code;
    code.w = 0;
    code.b.system = getSystemStatus() & 0xF;
    code.b.pm =  getPowermonStatus() & 0xF;
    code.b.therm = dev_thset_thermStatus() & 0xF;
    code.b.misc = getMiscStatus(dev) & 0xF;
    code.b.fpga = getFpgaStatus() & 0xF;
    code.b.pll = getPllStatus() & 0xF;
    return code;
}

SensorStatus getSystemStatus()
{
    const Devices *dev = getDevicesConst();
    const SensorStatus powermonStatus = getPowermonStatus();
    const SensorStatus temperatureStatus = dev_thset_thermStatus();
    const SensorStatus miscStatus = getMiscStatus(dev);
    const SensorStatus fpgaStatus = getFpgaStatus();
    const SensorStatus pllStatus = getPllStatus();
    const SensorStatus ad9516Status = get_auxpll_sensor_status();
    const SensorStatus digipotStatus = get_digipot_sensor_status();
//    const SensorStatus vxsiicStatus = pollVxsiicStatus(dev);
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
    if (digipotStatus > systemStatus)
        systemStatus = digipotStatus;
    if (ad9516Status > systemStatus)
        systemStatus = ad9516Status;

//    if (vxsiicStatus > systemStatus)
//        systemStatus = vxsiicStatus;
    return systemStatus;
}
