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
#include "app_task_powermon.h"

SensorStatus getMiscStatus(const Devices *d)
{
//    if (d->i2cmux.present != DEVICE_NORMAL)
//        return SENSOR_CRITICAL;
    if (d->eeprom_config.present != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getFpgaStatus(const Dev_fpga *d)
{
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->id != FPGA_DEVICE_ID)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getPllStatus(const Dev_ad9545 *d)
{
    if (d->fsm_state == PLL_STATE_ERROR || d->fsm_state == PLL_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.stable ||
            !d->status.sysclk.b.pll0_locked ||
            !d->status.sysclk.b.pll1_locked
            )
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getSystemStatus(const Devices *dev)
{
    const SensorStatus powermonStatus = getPowermonStatus(&dev->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&dev->thset);
    const SensorStatus miscStatus = getMiscStatus(dev);
    const SensorStatus fpgaStatus = getFpgaStatus(&dev->fpga);
    const SensorStatus pllStatus = getPllStatus(&dev->pll);
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
