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

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if ((d->sfpiic.present == DEVICE_NORMAL)
            && (d->sfpiic.present == DEVICE_NORMAL)
            && (d->vxsiic.present == DEVICE_NORMAL)
            && (d->eeprom_config.present == DEVICE_NORMAL)
            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->vxsiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->eeprom_config.present != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getFpgaStatus(const Dev_fpga *d)
{
    return get_fpga_sensor_status(d);
}

int getPllLockState(const Dev_ad9545 *d)
{
    return d->status.sysclk.b.stable
            && d->status.sysclk.b.pll0_locked
            && d->status.sysclk.b.pll1_locked;
}

SensorStatus getPllStatus(const Dev_ad9545 *d)
{
    if (d->fsm_state == PLL_STATE_ERROR || d->fsm_state == PLL_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
    if (!getPllLockState(d))
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getSystemStatus(void)
{
    const Devices *d = getDevicesConst();
    const SensorStatus powermonStatus = getPowermonStatus(&d->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&d->thset);
    const SensorStatus miscStatus = getMiscStatus(d);
    const SensorStatus fpgaStatus = getFpgaStatus(&d->fpga);
    const SensorStatus pllStatus = getPllStatus(&d->pll);
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
