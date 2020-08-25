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

#include "dev_ad9548.h"

#include <string.h>

#include "ad9548/ad9548.h"

void ad9548_update_pll_sensor_status(Dev_ad9548 *pll)
{
    if (DEVICE_NORMAL != pll->dev.device_status)
        pll->dev.sensor = SENSOR_UNKNOWN;
    if ((pll->priv.fsm_state != AD9548_STATE_RUN) || (!pll->priv.status.sysclk.b.locked))
        pll->dev.sensor = SENSOR_CRITICAL;
    if (!
        (pll->priv.status.DpllStat.b.dpll_freq_lock &&
         pll->priv.status.DpllStat.b.dpll_phase_lock))
        pll->dev.sensor = SENSOR_WARNING;
    pll->dev.sensor = SENSOR_NORMAL;
}

void ad9548_clear_status(Dev_ad9548 *d)
{
    memset(&d->priv.status.DpllStat, 0, sizeof(d->priv.status.DpllStat));
    memset(&d->priv.status.DpllStat2, 0, sizeof(d->priv.status.DpllStat2));
//    memset(&d->priv.status.sysclk, 0, sizeof(d->priv.status.sysclk));
}
