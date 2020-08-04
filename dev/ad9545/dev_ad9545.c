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

#include "dev_ad9545.h"

#include <string.h>

#include "ad9545/ad9545.h"

void update_pll_sensor_status(Dev_ad9545 *pll)
{
    if (DEVICE_NORMAL != pll->dev.device_status)
        pll->dev.sensor = SENSOR_UNKNOWN;
    if ((pll->priv.fsm_state != PLL_STATE_RUN) || (!pll->priv.status.sysclk.b.locked))
        pll->dev.sensor = SENSOR_CRITICAL;
    if (!pll->priv.status.dpll[0].lock_status.b.all_lock)
        pll->dev.sensor = SENSOR_WARNING;
    if (!pll->priv.status.dpll[1].lock_status.b.all_lock)
        pll->dev.sensor = SENSOR_WARNING;
    pll->dev.sensor = SENSOR_NORMAL;
}

void pll_ad9545_clear_status(Dev_ad9545 *d)
{
    memset(&d->priv.status.misc, 0, sizeof(d->priv.status.misc));
    memset(&d->priv.status.ref, 0, sizeof(d->priv.status.ref));
    memset(&d->priv.status.dpll, 0, sizeof(d->priv.status.dpll));
}
