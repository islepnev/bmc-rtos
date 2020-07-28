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
#include "dev_common_types.h"

bool getPllLockState(void)
{
    const DeviceBase *d = find_device(DEV_CLASS_PLL);
    if (!d)
        return false;
    Dev_ad9545_priv *priv = (Dev_ad9545_priv *)d->priv;
    return priv->status.sysclk.b.stable &&
           priv->status.sysclk.b.pll0_locked &&
           priv->status.sysclk.b.pll1_locked;
}

SensorStatus getPllStatus(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_PLL);
    if (!d)
        return SENSOR_UNKNOWN;
    Dev_ad9545_priv *priv = (Dev_ad9545_priv *)d->priv;

    if (priv->fsm_state == PLL_STATE_ERROR || priv->fsm_state == PLL_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->device_status != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!priv->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
    if (!getPllLockState())
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}
