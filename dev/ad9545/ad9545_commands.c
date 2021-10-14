/*
**    Copyright 2021 Ilia Slepnev
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

#include "ad9545_commands.h"

#include <stdlib.h>

#include "app_shared_data.h"
#include "ad9545/ad9545.h"
#include "dev_ad9545.h"
#include "log/log.h"

const int64_t phaser_step_ps = 2000;

typedef struct ad9545_phase_shift_fsm {
    int req;
    int64_t start_value;
    int64_t current_value;
    int64_t stop_value;
    int64_t step;
    Dev_ad9545 *dev;
} ad9545_phase_shift_fsm;

static ad9545_phase_shift_fsm phaser = {};

void setup_phaser(Dev_ad9545 *d)
{
    phaser.dev = d;
    phaser.start_value = d->priv.setup.dpll0.Phase_Offset;
    phaser.current_value = phaser.start_value;

    // adjust clock phase by part of 16ns clock
    phaser.stop_value = phaser.start_value;
    if (pll_clock_shift_command.denom != 0 && pll_clock_shift_command.numer != 0)
        phaser.stop_value += (uint64_t)16000 * pll_clock_shift_command.numer / pll_clock_shift_command.denom;

    phaser.req = pll_clock_shift_command.req;
    phaser.step = phaser_step_ps;
    if (phaser.start_value > phaser.stop_value)
        phaser.step = -llabs(phaser.step);
}

static bool phaser_complete()
{
    return phaser.current_value == phaser.stop_value;
}

static bool run_phaser(Dev_ad9545 *d)
{
    if (phaser_complete())
        return true;
    phaser.current_value += phaser.step;
    if (phaser.step > 0 && phaser.current_value > phaser.stop_value)
        phaser.current_value = phaser.stop_value;
    if (phaser.step < 0 && phaser.current_value < phaser.stop_value)
        phaser.current_value = phaser.stop_value;
    d->priv.setup.dpll0.Phase_Offset = phaser.current_value;
    if (!ad9545_dpll0_phase_shift(&d->dev.bus, &d->priv.setup)) {
        return false;
    }
    return true;
}

bool poll_ad9545_commands(Dev_ad9545 *d)
{
    if (phaser_complete()) {
        if (pll_clock_shift_command.req != pll_clock_shift_command.ack)
        setup_phaser(d);
        pll_clock_shift_command.ack = phaser.req;
    }
    if (phaser_complete())
        return true;

    if (!run_phaser(d))
        return false;

    // process
    log_printf(LOG_INFO, "%s: shift request #%d: %lld -> %lld, step %lld, current %lld ps",
               phaser.dev->dev.name,
               phaser.req,
               phaser.start_value,
               phaser.stop_value,
               phaser.step,
               phaser.current_value);

    return true;
}
