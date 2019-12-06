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

#include "app_task_pll_impl.h"
#include <string.h>
#include "cmsis_os.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "gpio.h"
#include "dev_ad9545.h"
#include "ad9545/ad9545.h"
#include "ad9545/ad9545_i2c_hal.h"
#include "logbuffer.h"
#include "pll_i2c_driver.h"

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void pll_task_init(void)
{
    Dev_ad9545 *d = get_dev_pll();
    init_ad9545_setup(&d->setup);
    ad9545_gpio_init();
    pll_i2c_init();
}

/*
1. Configure the system clock.
2. Configure the DPLL (digital PLL)
3. Configure the reference inputs.
4. Configure the output drivers
5. Configure the status pins (optional)
*/
void pll_task_run(void)
{
    Dev_ad9545 *d = get_dev_pll();
    const ad9545_state_t old_state = d->fsm_state;
    switch(d->fsm_state) {
    case PLL_STATE_INIT:
        if (enable_pll_run && enable_power) {
            if (ad9545_gpio_test()) {
                d->fsm_state = PLL_STATE_RESET;
            } else {
                log_put(LOG_ERR, "PLL GPIO test fail");
                d->fsm_state = PLL_STATE_ERROR;
            }
        }
        break;
    case PLL_STATE_RESET:
        ad9545_reset_i2c();
        ad9545_reset();
        osDelay(50);
        d->present = ad9545_detect() ? DEVICE_NORMAL : DEVICE_FAIL;
        if (DEVICE_NORMAL == d->present) {
            if (!ad9545_software_reset()) {
                d->fsm_state = PLL_STATE_ERROR;
                break;
            }
            d->fsm_state = PLL_STATE_SETUP_SYSCLK;
            break;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "PLL AD9545 not found");
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        break;
    case PLL_STATE_SETUP_SYSCLK:
        if (!ad9545_setup_sysclk(&d->setup.sysclk)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (!ad9545_calibrate_sysclk()) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->fsm_state = PLL_STATE_SYSCLK_WAITLOCK;
        break;
    case PLL_STATE_SYSCLK_WAITLOCK:
        if (d->status.sysclk.b.locked && d->status.sysclk.b.stable) {
            d->fsm_state = PLL_STATE_SETUP;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "PLL sysclock lock timeout");
            d->fsm_state = PLL_STATE_ERROR;
        }
        break;
    case PLL_STATE_SETUP:
        if (!ad9545_setup(&d->setup)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->fsm_state = PLL_STATE_RUN;
        break;
    case PLL_STATE_RUN:
        if (!d->status.sysclk.b.locked) {
            log_put(LOG_ERR, "PLL sysclock unlocked");
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->recoveryCount = 0;
        break;
    case PLL_STATE_ERROR:
        if (d->recoveryCount > 3) {
            d->fsm_state = PLL_STATE_FATAL;
            log_put(LOG_CRIT, "PLL fatal error");
            break;
        }
        if (stateTicks() > 1000) {
            d->recoveryCount++;
            d->fsm_state = PLL_STATE_INIT;
        }
        break;
    case PLL_STATE_FATAL:
        d->present = DEVICE_FAIL;
        if (stateTicks() > 2000) {
            // recover
            d->recoveryCount = 0;
            d->fsm_state = PLL_STATE_INIT;
        }
        break;
    default:
        d->fsm_state = PLL_STATE_INIT;
    }

    if (d->fsm_state != PLL_STATE_INIT &&
            d->fsm_state != PLL_STATE_RESET &&
            d->fsm_state != PLL_STATE_ERROR &&
            d->fsm_state != PLL_STATE_FATAL) {
        if (!ad9545_read_sysclk_status(&d->status)) {
            d->fsm_state = PLL_STATE_ERROR;
        }
    } else {
        d->status.sysclk.raw = 0;
    }
    if (d->fsm_state == PLL_STATE_RUN) {
        if (!ad9545_read_status(&d->status)) {
            d->fsm_state = PLL_STATE_ERROR;
        }
    } else {
        memset(&d->status.misc, 0, sizeof(d->status.misc));
        memset(&d->status.ref, 0, sizeof(d->status.ref));
        memset(&d->status.dpll, 0, sizeof(d->status.dpll));
    }
    int stateChanged = old_state != d->fsm_state;
    if (stateChanged) {
        stateStartTick = osKernelSysTick();
    }
    if (stateChanged && (old_state != PLL_STATE_RESET)) {
        if (d->fsm_state == PLL_STATE_ERROR) {
            log_put(LOG_ERR, "PLL interface error");
        }
        if (d->fsm_state == PLL_STATE_RUN) {
            log_put(LOG_INFO, "PLL started");
        }
    }
}
