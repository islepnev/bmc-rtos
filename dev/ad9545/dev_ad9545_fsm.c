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

#include "dev_ad9545_fsm.h"

#include <string.h>
#include <stdio.h>

#include "ad9545/ad9545.h"
#include "ad9545_commands.h"
#include "cmsis_os.h"
#include "dev_ad9545.h"
#include "log/log.h"

static uint32_t stateTicks(const Dev_ad9545_priv *p)
{
    return osKernelSysTick() - p->stateStartTick;
}

static AD9545_Status old_status = {};

enum {LOG_BUF_SIZE = 100 };
static char buf[LOG_BUF_SIZE];

void log_ad9545_status(Dev_ad9545 *d)
{
    snprintf(buf, sizeof(buf), "%s:", d->dev.name);
    bool warn = false;
    bool info = false;
    if (old_status.ref[0].b.valid && ! d->priv.status.ref[0].b.valid) {
        warn = true;
        strncat(buf, " REFA invalid", sizeof(buf) - strlen(buf) - 1);
    }
    if (!old_status.ref[0].b.valid && d->priv.status.ref[0].b.valid) {
        info = true;
        strncat(buf, " REFA valid", sizeof(buf) - strlen(buf) - 1);
    }
    if (old_status.ref[2].b.valid && ! d->priv.status.ref[2].b.valid) {
        warn = true;
        strncat(buf, " REFB invalid", sizeof(buf) - strlen(buf) - 1);
    }
    if (!old_status.ref[2].b.valid && d->priv.status.ref[2].b.valid) {
        info = true;
        strncat(buf, " REFB valid", sizeof(buf) - strlen(buf) - 1);
    }
    if (old_status.sysclk.b.pll0_locked && ! d->priv.status.sysclk.b.pll0_locked) {
        warn = true;
        strncat(buf, " DPLL0 unlocked", sizeof(buf) - strlen(buf) - 1);
    }
    if (!old_status.sysclk.b.pll0_locked && d->priv.status.sysclk.b.pll0_locked) {
        info = true;
        strncat(buf, " DPLL0 locked", sizeof(buf) - strlen(buf) - 1);
    }
    if (warn || info)
        log_put(warn ? LOG_WARNING : LOG_INFO, buf);
    old_status = d->priv.status;
}

/*
1. Configure the system clock.
2. Configure the DPLL (digital PLL)
3. Configure the reference inputs.
4. Configure the output drivers
5. Configure the status pins (optional)
*/
void dev_ad9545_run(Dev_ad9545 *d, bool enable)
{
    BusInterface *bus = &d->dev.bus;
    if (!enable) {
        if (d->priv.fsm_state != AD9545_STATE_INIT) {
            d->priv.fsm_state = AD9545_STATE_INIT;
            d->dev.device_status = DEVICE_UNKNOWN;
            pll_ad9545_clear_status(&d->priv.status);
            pll_ad9545_clear_status(&old_status);
            log_put(LOG_INFO, "PLL AD9545 shutdown");
        }
        return;
    }
    const ad9545_state_t old_state = d->priv.fsm_state;
    switch(d->priv.fsm_state) {
    case AD9545_STATE_INIT:
        if (ad9545_gpio_test(bus)) {
            d->priv.fsm_state = AD9545_STATE_RESET;
        } else {
            log_put(LOG_ERR, "PLL AD9545 GPIO test fail");
            d->priv.fsm_state = AD9545_STATE_ERROR;
        }
        break;
    case AD9545_STATE_RESET:
        ad9545_reset(bus);
        osDelay(50);
        d->dev.device_status = ad9545_detect(bus) ? DEVICE_NORMAL : DEVICE_FAIL;
        if (DEVICE_NORMAL == d->dev.device_status) {
            if (!ad9545_software_reset(bus)) {
                d->priv.fsm_state = AD9545_STATE_ERROR;
                break;
            }
            d->priv.fsm_state = AD9545_STATE_SETUP_SYSCLK;
            break;
        }
        if (stateTicks(&d->priv) > 2000) {
            log_put(LOG_ERR, "PLL AD9545 not found");
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        break;
    case AD9545_STATE_SETUP_SYSCLK:
        if (!ad9545_setup_sysclk(bus, &d->priv.setup.sysclk)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        if (!ad9545_calibrate_sysclk(bus)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = AD9545_STATE_SYSCLK_WAITLOCK;
        break;
    case AD9545_STATE_SYSCLK_WAITLOCK:
        if (d->priv.status.sysclk.b.locked && d->priv.status.sysclk.b.stable) {
            d->priv.fsm_state = AD9545_STATE_SETUP;
        }
        if (stateTicks(&d->priv) > 2000) {
            log_put(LOG_ERR, "PLL AD9545 sysclock lock timeout");
            d->priv.fsm_state = AD9545_STATE_ERROR;
        }
        break;
    case AD9545_STATE_SETUP:
        if (!ad9545_setup(bus, &d->priv.setup)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = AD9545_STATE_RUN;
        break;
    case AD9545_STATE_RUN:
        if (!d->priv.status.sysclk.b.locked) {
            log_put(LOG_ERR, "PLL AD9545 sysclock unlocked");
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        if (!poll_ad9545_commands(d)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
            break;
        }
        d->priv.recoveryCount = 0;
        break;
    case AD9545_STATE_ERROR:
        if (d->priv.recoveryCount > 3) {
            d->priv.fsm_state = AD9545_STATE_FATAL;
            log_put(LOG_CRIT, "PLL AD9545 fatal error");
            break;
        }
        if (stateTicks(&d->priv) > 1000) {
            d->priv.recoveryCount++;
            d->priv.fsm_state = AD9545_STATE_INIT;
        }
        break;
    case AD9545_STATE_FATAL:
        d->dev.device_status = DEVICE_FAIL;
        if (stateTicks(&d->priv) > 2000) {
            // recover
            d->priv.recoveryCount = 0;
            d->priv.fsm_state = AD9545_STATE_INIT;
        }
        break;
    default:
        d->priv.fsm_state = AD9545_STATE_INIT;
    }

    if (d->priv.fsm_state != AD9545_STATE_INIT &&
            d->priv.fsm_state != AD9545_STATE_RESET &&
            d->priv.fsm_state != AD9545_STATE_ERROR &&
            d->priv.fsm_state != AD9545_STATE_FATAL) {
        if (!ad9545_read_sysclk_status(bus, &d->priv.status)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
        }
    } else {
        d->priv.status.sysclk.raw = 0;
    }
    if (d->priv.fsm_state == AD9545_STATE_RUN) {
        if (!ad9545_read_status(bus, &d->priv.status)) {
            d->priv.fsm_state = AD9545_STATE_ERROR;
        }
    } else {
        pll_ad9545_clear_status(&d->priv.status);
        pll_ad9545_clear_status(&old_status);
    }
    ad9545_update_pll_sensor_status(d);
    log_ad9545_status(d);

    int stateChanged = old_state != d->priv.fsm_state;
    if (stateChanged) {
        d->priv.stateStartTick = osKernelSysTick();
    }
    if (stateChanged && (old_state != AD9545_STATE_RESET)) {
        if (d->priv.fsm_state == AD9545_STATE_ERROR) {
            log_put(LOG_ERR, "PLL AD9545 interface error");
        }
        if (d->priv.fsm_state == AD9545_STATE_RUN) {
            log_put(LOG_INFO, "PLL AD9545 started");
        }
    }
}
