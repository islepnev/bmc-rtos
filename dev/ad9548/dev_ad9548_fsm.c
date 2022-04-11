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

#include "dev_ad9548_fsm.h"

#include <string.h>

#include "ad9548.h"
#include "ad9548_setup.h"
#include "ad9548_status.h"
#include "cmsis_os.h"
#include "dev_ad9548.h"
#include "log/log.h"

static uint32_t stateTicks(const Dev_ad9548_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

/*
1. Configure the system clock.
2. Configure the DPLL (digital PLL)
3. Configure the reference inputs.
4. Configure the output drivers
5. Configure the status pins (optional)
*/
void dev_ad9548_run(Dev_ad9548 *d, bool enable)
{
    BusInterface *bus = &d->dev.bus;
    if (!enable) {
        if (d->priv.fsm_state != AD9548_STATE_INIT) {
            d->priv.fsm_state = AD9548_STATE_INIT;
            d->dev.device_status = DEVICE_UNKNOWN;
            ad9548_clear_status(d);
            ad9548_write_reset_pin(bus, true);
            log_put(LOG_INFO, "PLL AD9548 shutdown");
        }
        return;
    }
    const ad9548_state_t old_state = d->priv.fsm_state;
    switch(d->priv.fsm_state) {
    case AD9548_STATE_INIT:
        ad9548_write_reset_pin(bus, false);
        if (ad9548_gpio_test(bus)) {
            d->priv.fsm_state = AD9548_STATE_RESET;
        } else {
            log_put(LOG_ERR, "PLL AD9548 GPIO test fail");
            d->priv.fsm_state = AD9548_STATE_ERROR;
        }
        break;
    case AD9548_STATE_RESET:
        ad9548_write_reset_pin(bus, true);
        ad9548_write_reset_pin(bus, false);
        if (!ad9548_software_reset(bus)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
            break;
        }
        d->dev.device_status = ad9548_detect(bus) ? DEVICE_NORMAL : DEVICE_FAIL;
        if (DEVICE_NORMAL == d->dev.device_status) {
            d->priv.fsm_state = AD9548_STATE_SETUP_SYSCLK;
            break;
        }
        if (stateTicks(&d->priv) > 2000) {
            log_put(LOG_ERR, "PLL AD9548 not found");
            d->priv.fsm_state = AD9548_STATE_ERROR;
            break;
        }
        break;
    case AD9548_STATE_SETUP_SYSCLK:
        if (!ad9548_setup_sysclk(bus, &d->priv.setup)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
            break;
        }
        if (!ad9548_calibrate_sysclk(bus, &d->priv.setup)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = AD9548_STATE_SYSCLK_WAITLOCK;
        break;
    case AD9548_STATE_SYSCLK_WAITLOCK:
        if (ad9548_sysclk_is_locked(d->priv.status.sysclk)) {
            d->priv.fsm_state = AD9548_STATE_SETUP;
        }
        if (stateTicks(&d->priv) > 2000) {
            log_put(LOG_ERR, "PLL AD9548 sysclock lock timeout");
            d->priv.fsm_state = AD9548_STATE_ERROR;
        }
        break;
    case AD9548_STATE_SETUP:
        if (!ad9548_setup(bus, &d->priv.setup)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = AD9548_STATE_RUN;
        break;
    case AD9548_STATE_RUN:
        if (!d->priv.status.sysclk.b.locked) {
            log_put(LOG_ERR, "PLL AD9548 sysclock unlocked");
            d->priv.fsm_state = AD9548_STATE_SYSCLK_WAITLOCK;
            break;
        }
        d->priv.recoveryCount = 0;
        break;
    case AD9548_STATE_ERROR:
        if (d->priv.recoveryCount > 3) {
            d->priv.fsm_state = AD9548_STATE_FATAL;
            log_put(LOG_CRIT, "PLL AD9548 fatal error");
            break;
        }
        if (stateTicks(&d->priv) > 1000) {
            d->priv.recoveryCount++;
            d->priv.fsm_state = AD9548_STATE_INIT;
        }
        break;
    case AD9548_STATE_FATAL:
        d->dev.device_status = DEVICE_FAIL;
        if (stateTicks(&d->priv) > 2000) {
            // recover
            d->priv.recoveryCount = 0;
            d->priv.fsm_state = AD9548_STATE_INIT;
        }
        break;
    default:
        d->priv.fsm_state = AD9548_STATE_INIT;
    }

    bool state_detected =
        (d->priv.fsm_state != AD9548_STATE_INIT &&
         d->priv.fsm_state != AD9548_STATE_RESET &&
         d->priv.fsm_state != AD9548_STATE_ERROR &&
         d->priv.fsm_state != AD9548_STATE_FATAL);
    if (state_detected) {
        if (!ad9548_read_sysclk_status(bus, &d->priv.status)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
        }
    } else {
        d->priv.status.sysclk.raw = 0;
    }
    if (d->priv.fsm_state == AD9548_STATE_RUN) {
        if (!ad9548_poll_irq_state(bus, &d->priv.status)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
        }
        if (!ad9548_read_status(bus, &d->priv.status)) {
            d->priv.fsm_state = AD9548_STATE_ERROR;
        }
    } else {
        ad9548_clear_status(d);
    }
    ad9548_update_pll_sensor_status(d);

    int stateChanged = old_state != d->priv.fsm_state;
    if (stateChanged) {
        d->priv.stateStartTick = osKernelSysTick();
    }
    if (stateChanged && (old_state != AD9548_STATE_RESET)) {
        if (d->priv.fsm_state == AD9548_STATE_ERROR) {
            log_put(LOG_ERR, "PLL AD9548 interface error");
        }
        if (d->priv.fsm_state == AD9548_STATE_RUN) {
            log_put(LOG_INFO, "PLL AD9548 started");
        }
    }
}
