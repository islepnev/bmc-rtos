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

#include "app_task_auxpll_impl.h"

#include "ad9516/ad9516_spi_hal.h"
#include "cmsis_os.h"
#include "dev_auxpll.h"
#include "dev_auxpll_types.h"
#include "log/log.h"

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void auxpll_clear_status(Dev_auxpll *d)
{
    d->priv.status.pll_readback.raw = 0;
}

static int old_enable = 0;

void auxpll_task_run(Dev_auxpll *d, bool enable)
{
    if (old_enable != enable) {
        old_enable = enable;
    }
    if (!enable) {
        if (d->priv.fsm_state != AUXPLL_STATE_INIT) {
            d->priv.fsm_state = AUXPLL_STATE_INIT;
            d->dev.device_status = DEVICE_UNKNOWN;
            ad9516_disable_interface();
            auxpll_clear_status(d);
            log_put(LOG_INFO, "PLL AD9516 shutdown");
        }
        return;
    }
    const AuxPllState old_state = d->priv.fsm_state;
    switch(d->priv.fsm_state) {
    case AUXPLL_STATE_INIT:
        ad9516_enable_interface();
        auxpll_clear_status(d);
        d->priv.fsm_state = AUXPLL_STATE_RESET;
        break;
    case AUXPLL_STATE_RESET:
        if (!auxpllSoftwareReset()) {
            d->priv.fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        auxpllDetect(d);
        if (DEVICE_NORMAL == d->dev.device_status) {
            d->priv.fsm_state = AUXPLL_STATE_SETUP;
            break;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "AUXPLL AD9516 not found");
            d->priv.fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        break;
    case AUXPLL_STATE_SETUP:
        if (! auxpllSetup(d)) {
            d->priv.fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = AUXPLL_STATE_RUN;
        break;
    case AUXPLL_STATE_RUN:
        if (!enable) {
            d->priv.fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        d->priv.recoveryCount = 0;
        break;
    case AUXPLL_STATE_ERROR:
        if (d->priv.recoveryCount > 3) {
            d->priv.fsm_state = AUXPLL_STATE_FATAL;
            log_put(LOG_CRIT, "AUXPLL AD9516 fatal error");
            break;
        }
        if (stateTicks() > 1000) {
            ad9516_disable_interface();
            d->priv.recoveryCount++;
            d->priv.fsm_state = AUXPLL_STATE_INIT;
        }
        break;
    case AUXPLL_STATE_FATAL:
        d->dev.device_status = DEVICE_FAIL;
        if (stateTicks() > 2000) {
            // recover
            d->priv.recoveryCount = 0;
            d->priv.fsm_state = AUXPLL_STATE_INIT;
        }
        break;
    default:
        d->priv.fsm_state = AUXPLL_STATE_INIT;
    }

    if (d->priv.fsm_state != AUXPLL_STATE_INIT &&
            d->priv.fsm_state != AUXPLL_STATE_RESET &&
            d->priv.fsm_state != AUXPLL_STATE_ERROR &&
            d->priv.fsm_state != AUXPLL_STATE_FATAL) {
    }
    if (d->priv.fsm_state == AUXPLL_STATE_RUN) {
        if (! auxpllReadStatus(d)) {
            d->priv.fsm_state = AUXPLL_STATE_ERROR;
        }
    } else {
        auxpll_clear_status(d);
    }
    int stateChanged = old_state != d->priv.fsm_state;
    if (stateChanged) {
        stateStartTick = osKernelSysTick();
    }
    if (stateChanged && (old_state != AUXPLL_STATE_RESET)) {
        if (d->priv.fsm_state == AUXPLL_STATE_ERROR) {
            log_put(LOG_ERR, "AUXPLL AD9516 interface error");
        }
        if (d->priv.fsm_state == AUXPLL_STATE_RUN) {
            log_put(LOG_INFO, "AUXPLL AD9516 started");
        }
    }
}
