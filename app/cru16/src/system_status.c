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
    DeviceStatus status = DEVICE_FAIL;
    if ((d->sfpiic.dev.device_status == DEVICE_NORMAL)
            && (d->sfpiic.dev.device_status == DEVICE_NORMAL)
            && (d->eeprom_config.dev.device_status == DEVICE_NORMAL)
            && (d->pll.dev.device_status == DEVICE_NORMAL)
            && (d->fpga.dev.device_status == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.dev.device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->eeprom_config.dev.device_status != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getFpgaStatus(const Dev_fpga *d)
{
    return get_fpga_sensor_status(d);
}

encoded_system_status_t encode_system_status(const Devices *dev)
{
    encoded_system_status_t code;
    code.w = 0;
    code.b.system = getSystemStatus() & 0xF;
    code.b.pm =  getPowermonStatus(&dev->pm) & 0xF;
    code.b.therm = dev_thset_thermStatus(&dev->thset) & 0xF;
    code.b.misc = getMiscStatus(dev) & 0xF;
    code.b.fpga = getFpgaStatus(&dev->fpga) & 0xF;
    code.b.pll = getPllStatus() & 0xF;
    return code;
}

SensorStatus getSystemStatus(void)
{
    const Devices *d = getDevicesConst();
    const SensorStatus powermonStatus = getPowermonStatus(&d->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&d->thset);
    const SensorStatus miscStatus = getMiscStatus(d);
    const SensorStatus fpgaStatus = getFpgaStatus(&d->fpga);
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
