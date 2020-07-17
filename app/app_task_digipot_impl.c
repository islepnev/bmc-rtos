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
#include "app_task_digipot_impl.h"

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os.h"

#include "dev_pot.h"
#include "bsp_digipot.h"
#include "dev_common_types.h"
#include "logbuffer.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "error_handler.h"
#include "commands.h"

#include "cmsis_os.h"

void pot_command_process(Dev_digipots *d, const CommandPots *cmd)
{
    if (!cmd || cmd->arg >= DEV_DIGIPOT_COUNT)
        return;
    Dev_ad5141 *p = &d->pot[cmd->arg];
    switch (cmd->command_id) {
    case COMMAND_POTS_RESET:
        dev_ad5141_reset(p);
        break;
    case COMMAND_POTS_INC:
        dev_ad5141_inc(p);
        break;
    case COMMAND_POTS_DEC:
        dev_ad5141_dec(p);
        break;
    case COMMAND_POTS_WRITE:
        dev_ad5141_write(p);
        break;
    default:
        break;
    }
}

static const int POT_MAX_MAIL_BATCH = 10;

void pot_check_mail(Dev_digipots *d)
{
    if (!mq_cmd_pots_id)
        Error_Handler();
    for (int i=0; i<POT_MAX_MAIL_BATCH; i++) {
        osEvent event = osMailGet(mq_cmd_pots_id, 0);
        if (osEventMail != event.status) {
            return;
        }
        CommandPots *mail = (CommandPots *) event.value.p;
        if (!mail)
            Error_Handler();
        pot_command_process(d, mail);
        osMailFree(mq_cmd_pots_id, mail);
    }
}

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

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

void task_digipot_run(void)
{
    Dev_digipots *d = get_dev_digipots();
    switch (state) {
    case STATE_INIT: {
        struct_pots_init(get_dev_digipots());
        state = STATE_DETECT;
        break;
    }
    case STATE_DETECT: {
        int pots_detected = pot_detect(d);
        if (pots_detected > 0)
            state = STATE_RUN;
        else
            osDelay(100);
        break;
    }
    case STATE_RUN:
        pot_read_rdac_all(d);
        pot_check_mail(d);
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
