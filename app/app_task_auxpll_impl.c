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
#include "cmsis_os.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "dev_auxpll.h"
#include "ad9516_spi_hal.h"
#include "logbuffer.h"

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

static int old_enable = 0;

void auxpll_task_run(void)
{
    bool enable = enable_power && system_power_present;
    if (old_enable != enable) {
        old_enable = enable;
        if (enable)
            ad9516_enable_interface();
        else
            ad9516_disable_interface();
    }
    Dev_auxpll *d = get_dev_auxpll();
    const AuxPllState old_state = d->fsm_state;
    switch(d->fsm_state) {
    case AUXPLL_STATE_INIT:
        d->status.pll_readback.raw = 0;
        if (enable)
            d->fsm_state = AUXPLL_STATE_RESET;
        break;
    case AUXPLL_STATE_RESET:
        if (!enable) {
            d->fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        if (!auxpllSoftwareReset()) {
            break;
        }
        auxpllDetect(d);
        if (DEVICE_NORMAL == d->present) {
            d->fsm_state = AUXPLL_STATE_SETUP;
            break;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "AUXPLL AD9516 not found");
            d->fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        break;
    case AUXPLL_STATE_SETUP:
        if (DEV_OK != auxpllSetup(d)) {
            d->fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        d->fsm_state = AUXPLL_STATE_RUN;
        break;
    case AUXPLL_STATE_RUN:
        if (!enable) {
            d->fsm_state = AUXPLL_STATE_ERROR;
            break;
        }
        d->recoveryCount = 0;
        break;
    case AUXPLL_STATE_ERROR:
        ad9516_disable_interface();
        if (d->recoveryCount > 3) {
            d->fsm_state = AUXPLL_STATE_FATAL;
            log_put(LOG_CRIT, "AUXPLL AD9516 fatal error");
            break;
        }
        if (stateTicks() > 1000) {
            d->recoveryCount++;
            d->fsm_state = AUXPLL_STATE_INIT;
        }
        break;
    case AUXPLL_STATE_FATAL:
        d->present = DEVICE_FAIL;
        if (stateTicks() > 2000) {
            // recover
            d->recoveryCount = 0;
            d->fsm_state = AUXPLL_STATE_INIT;
        }
        break;
    default:
        d->fsm_state = AUXPLL_STATE_INIT;
    }

    if (d->fsm_state != AUXPLL_STATE_INIT &&
            d->fsm_state != AUXPLL_STATE_RESET &&
            d->fsm_state != AUXPLL_STATE_ERROR &&
            d->fsm_state != AUXPLL_STATE_FATAL) {
    }
    if (d->fsm_state == AUXPLL_STATE_RUN) {
        if (DEV_OK != auxpllReadStatus(d)) {
            d->fsm_state = AUXPLL_STATE_ERROR;
        }
    } else {
        d->status.pll_readback.raw = 0;
    }
    int stateChanged = old_state != d->fsm_state;
    if (stateChanged) {
        stateStartTick = osKernelSysTick();
    }
    if (stateChanged && (old_state != AUXPLL_STATE_RESET)) {
        if (d->fsm_state == AUXPLL_STATE_ERROR) {
            log_put(LOG_ERR, "AUXPLL AD9516 interface error");
        }
        if (d->fsm_state == AUXPLL_STATE_RUN) {
            log_put(LOG_INFO, "AUXPLL AD9516 started");
        }
    }
}
