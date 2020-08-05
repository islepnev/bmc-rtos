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

#include "commands_menu.h"

#include <string.h>

#include "cmsis_os.h"
#include "commands.h"
#include "ipc.h"
#include "keysyms.h"
#include "log/log.h"

static void menu_send_command(menu_command_id_t id, uint32_t arg)
{
    CommandMenu *cmd = osMailAlloc(mq_cmd_menu_id, osWaitForever);
    if (!cmd) {
        goto err;
    }
    cmd->command_id = id;
    cmd->arg = arg;
    if (osOK != osMailPut(mq_cmd_menu_id, cmd))
        goto err;
    return;
err:
    log_printf(LOG_WARNING, "menu command ignored: %d, %lu", id, arg);
}

static void menu_action(menu_command_id_t command_id)
{
    menu_send_command(command_id, 0);
}

bool menu_handle_escape_seq(const char *str)
{
    bool handled = false;
    if (0 == strcmp(str, ESC_CODE_LEFT)) {
        menu_action(COMMAND_MENU_BACK);
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_UP)) {
        menu_action(COMMAND_MENU_UP);
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_DOWN)) {
        menu_action(COMMAND_MENU_DOWN);
        handled = true;
    }
    return handled;
}

void menu_screen_handle_key(const char ch)
{
    if (ch >= '0' || ch <= '9') {
        menu_action(COMMAND_MENU_DIGIT_0 + ch - '0');
    }
    if (ch == 'u' || ch == 'U' || ch == 'p' || ch == 'P') {
        menu_action(COMMAND_MENU_UP);
    }
    if (ch == 'd' || ch == 'D' || ch == 'n' || ch == 'N') {
        menu_action(COMMAND_MENU_DOWN);
    }
    if (ch == ASCII_CR || ch == ASCII_LF) {
        menu_action(COMMAND_MENU_ENTER);
    }
    if (ch == 'e' || ch == 'E') {
        menu_action(COMMAND_MENU_ENTER);
    }
    if (ch == 'b' || ch == 'B' || ch == ASCII_DEL) {
        menu_action(COMMAND_MENU_BACK);
    }
    if (ch == 'q' || ch == 'Q') {
        menu_action(COMMAND_MENU_HOME);
    }
}
