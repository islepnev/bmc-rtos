/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "dev_digipot_fsm.h"

#include "dev_digipot.h"
#include "dev_digipot_commands.h"
#include "bsp_digipot.h"
#include "dev_common_types.h"
#include "logbuffer.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "error_handler.h"
#include "commands.h"

#include "cmsis_os.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;

typedef enum {
    STATE_INIT,
    STATE_DETECT,
    STATE_RUN,
    STATE_ERROR
} state_t;

static state_t state = STATE_INIT;
static state_t old_state = STATE_INIT;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

void dev_digipot_run(struct Dev_digipots *d)
{
    switch (state) {
    case STATE_INIT: {
        //dev_digipots_priv_init(&d->priv);
        d->priv.count = 0;
        state = STATE_DETECT;
        break;
    }
    case STATE_DETECT: {
        int pots_detected_1 = digipot_detect(d);
        if (pots_detected_1 == 0) {
            osDelay(100);
            break;
        }
        int pots_detected_2 = digipot_detect(d);
        if (pots_detected_2 == pots_detected_1) {
            log_printf(LOG_INFO, "Found %d digipots", pots_detected_2);
            state = STATE_RUN;
            break;
        }
        osDelay(100);
        break;
    }
    case STATE_RUN:
        digipot_read_rdac_all(d);
        digipot_check_mail(&d->priv);
        break;

    case STATE_ERROR:
        if (old_state != state) {
            log_printf(LOG_ERR, "Digipot error");
        }
        if (stateTicks() > ERROR_DELAY_TICKS) {
            state = STATE_INIT;
        }
        break;
    }
    if (old_state != state) {
        stateStartTick = osKernelSysTick();
    }
}
