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

#include <assert.h>

#include "dev_digipot.h"
#include "ipc.h"

bool digipot_process_command(Dev_digipots_priv *d, const CommandDigipots *cmd)
{
    if (!cmd || cmd->arg >= MAX_DIGIPOT_COUNT)
        return false;
    Dev_ad5141 *p = &d->pot[cmd->arg];
    switch (cmd->command_id) {
    case COMMAND_DIGIPOTS_RESET:
        return dev_ad5141_reset(p);
    case COMMAND_DIGIPOTS_INC:
        return dev_ad5141_inc(p);
    case COMMAND_DIGIPOTS_DEC:
        return dev_ad5141_dec(p);
    case COMMAND_DIGIPOTS_WRITE:
        return dev_ad5141_write(p);
    default:
        break;
    }
    return true;
}

static const int POT_MAX_MAIL_BATCH = 10;

bool digipot_check_mail(Dev_digipots_priv *d)
{
    bool ok = true;
    assert(mq_cmd_digipots_id);
    if (!mq_cmd_digipots_id)
        return false;
    for (int i=0; i<POT_MAX_MAIL_BATCH; i++) {
        osEvent event = osMailGet(mq_cmd_digipots_id, 0);
        if (osEventMail != event.status) {
            return true;
        }
        CommandDigipots *mail = (CommandDigipots *) event.value.p;
        assert(mail);
        if (!mail)
            return false;
        ok &= digipot_process_command(d, mail);
        ok &= (osOK == osMailFree(mq_cmd_digipots_id, mail));
    }
    return ok;
}
