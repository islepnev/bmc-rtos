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

#include <stdint.h>
#include "dev_common_types.h"
#include "dev_pll.h"

SensorStatus get_pll_sensor_status(const Dev_pll *pll)
{
    if (DEVICE_NORMAL != pll->present)
        return SENSOR_UNKNOWN;
    if ((pll->fsm_state != PLL_STATE_RUN) || (!pll->status.sysclk.b.locked))
        return SENSOR_CRITICAL;
    if (!pll->status.dpll[0].lock_status.b.all_lock)
        return SENSOR_WARNING;
    if (!pll->status.dpll[1].lock_status.b.all_lock)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}
