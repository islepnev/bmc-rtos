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

#include "dev_digipot_commands.h"

#include "dev_digipot.h"
#include "error_handler.h"
#include "commands.h"

#include "cmsis_os.h"

void digipot_process_command(Dev_digipots_priv *d, const CommandDigipots *cmd)
{
    if (!cmd || cmd->arg >= DEV_DIGIPOT_COUNT)
        return;
    Dev_ad5141 *p = &d->pot[cmd->arg];
    switch (cmd->command_id) {
    case COMMAND_DIGIPOTS_RESET:
        dev_ad5141_reset(p);
        break;
    case COMMAND_DIGIPOTS_INC:
        dev_ad5141_inc(p);
        break;
    case COMMAND_DIGIPOTS_DEC:
        dev_ad5141_dec(p);
        break;
    case COMMAND_DIGIPOTS_WRITE:
        dev_ad5141_write(p);
        break;
    default:
        break;
    }
}

static const int POT_MAX_MAIL_BATCH = 10;

void digipot_check_mail(Dev_digipots_priv *d)
{
    if (!mq_cmd_digipots_id)
        Error_Handler();
    for (int i=0; i<POT_MAX_MAIL_BATCH; i++) {
        osEvent event = osMailGet(mq_cmd_digipots_id, 0);
        if (osEventMail != event.status) {
            return;
        }
        CommandDigipots *mail = (CommandDigipots *) event.value.p;
        if (!mail)
            Error_Handler();
        digipot_process_command(d, mail);
        osMailFree(mq_cmd_digipots_id, mail);
    }
}
