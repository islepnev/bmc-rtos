/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "system_status_common.h"

#include "ad9545/dev_ad9545.h"
#include "ad9548/dev_ad9548.h"
#include "dev_common_types.h"
#include "devicelist.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "fpga/dev_fpga_types.h"

//SensorStatus get_eepromConfig_status(void)
//{
//    const DeviceBase *d = find_device_const(DEV_CLASS_EEPROM);
//    if (!d || !d->priv)
//        return SENSOR_UNKNOWN;
//    const Dev_eeprom_config_priv *priv = (const Dev_eeprom_config_priv *)device_priv_const(d);

//    if (priv->fsm_state == EEPROM_CONFIG_STATE_ERROR)
//        return SENSOR_CRITICAL;
//    if (d->device_status != DEVICE_NORMAL)
//        return SENSOR_CRITICAL;
//    if (!getPllLockState())
//        return SENSOR_WARNING;
//    return SENSOR_NORMAL;
//}

SensorStatus getFpgaStatus(void)
{
    return get_fpga_sensor_status();
}

bool getPllLockState(void)
{
#if ENABLE_AD9545
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9545);
    if (!d || !d->priv)
        return false;
    const Dev_ad9545_priv *priv = (const Dev_ad9545_priv *)d->priv;
    return priv->status.sysclk.b.stable &&
           priv->status.sysclk.b.pll0_locked &&
           priv->status.sysclk.b.pll1_locked;
#endif
#if ENABLE_AD9548
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9548);
    if (!d || !d->priv)
        return false;
    const Dev_ad9548_priv *priv = (const Dev_ad9548_priv *)d->priv;
    return priv->status.sysclk.b.stable &&
           priv->status.DpllStat.b.dpll_freq_lock &&
           priv->status.DpllStat.b.dpll_phase_lock;
#endif
}

SensorStatus getPllStatus(void)
{
#if ENABLE_AD9545
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9545);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;
    const Dev_ad9545_priv *priv = (const Dev_ad9545_priv *)d->priv;

    if (priv->fsm_state == AD9545_STATE_ERROR || priv->fsm_state == AD9545_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!priv->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
#endif
#if ENABLE_AD9548
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9548);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;
    const Dev_ad9548_priv *priv = (const Dev_ad9548_priv *)d->priv;

    if (priv->fsm_state == AD9548_STATE_ERROR || priv->fsm_state == AD9548_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!priv->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
#endif
    if (!getPllLockState())
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}
