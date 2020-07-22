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
#include "app_task_vxsiic_impl.h"
#include "dev_fpga.h"
#include "dev_thset.h"

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if ((d->sfpiic.present == DEVICE_NORMAL)
            // && (d->eeprom_vxspb.present == DEVICE_NORMAL)
            && (d->eeprom_config.present == DEVICE_NORMAL)
            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.present == DEVICE_FAIL)
        return SENSOR_WARNING;
    if (d->eeprom_config.present == DEVICE_FAIL)
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

SensorStatus pollVxsiicStatus(Devices *dev)
{
    static vxsiic_i2c_stats_t vxsiic_i2c_stats_save = {0};
    const vxsiic_i2c_stats_t * vxsiic_i2c_stats = get_vxsiic_i2c_stats_ptr();
    uint32_t vxsiic_errors = vxsiic_i2c_stats->errors - vxsiic_i2c_stats_save.errors;
    const SensorStatus vxsiicStatus = vxsiic_errors ? SENSOR_CRITICAL : (vxsiic_i2c_stats->errors) ? SENSOR_WARNING : SENSOR_NORMAL;
    vxsiic_i2c_stats_save = *vxsiic_i2c_stats;
    return vxsiicStatus;
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
    code.b.pll = getPllStatus(&dev->pll) & 0xF;
    return code;
}

SensorStatus getSystemStatus()
{
    const Devices *dev = getDevicesConst();
    const SensorStatus powermonStatus = getPowermonStatus(&dev->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&dev->thset);
    const SensorStatus miscStatus = getMiscStatus(dev);
    const SensorStatus fpgaStatus = getFpgaStatus(&dev->fpga);
    const SensorStatus pllStatus = getPllStatus(&dev->pll);
    const SensorStatus ad9516Status = get_auxpll_sensor_status(&dev->auxpll);
    const SensorStatus digipotStatus = get_digipot_sensor_status(&dev->pots);
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
#ifdef BOARD_TDC64
    if (ad9516Status > systemStatus)
        systemStatus = ad9516Status;
#endif

//    if (vxsiicStatus > systemStatus)
//        systemStatus = vxsiicStatus;
    return systemStatus;
}
