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

#include "dev_auxpll_types.h"

#include <stdint.h>

#include "dev_auxpll.h"
#include "dev_common_types.h"
#include "devicebase.h"

SensorStatus get_auxpll_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9516);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;
    const Dev_auxpll_priv *priv = (const Dev_auxpll_priv *)device_priv_const(d);

    if (DEVICE_NORMAL != d->device_status)
        return SENSOR_UNKNOWN;
    // TODO
//    if ((pll->fsm_state != PLL_STATE_RUN) || (!pll->status.sysclk.b.locked))
//        return SENSOR_CRITICAL;
//    if (!pll->status.dpll[0].lock_status.b.all_lock)
//        return SENSOR_WARNING;
    if (!priv->status.pll_readback.b.dlock)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}
