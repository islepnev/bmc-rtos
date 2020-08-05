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

#include "commands_digipot.h"

#include <string.h>

#include "app_shared_data.h"
#include "commands.h"
#include "digipot/dev_digipot.h"
#include "keysyms.h"
#include "log/log.h"

enum {KEYBUF_SIZE = 3};
static char keybuf[KEYBUF_SIZE+1] = {0};
static int keybuf_len = 0;
static void put_keybuf(char ch)
{
    if (keybuf_len < KEYBUF_SIZE) {
        keybuf[keybuf_len] = ch;
        keybuf_len++;
    }
}

static void clear_keybuf(void)
{
    for (int i=0; i<KEYBUF_SIZE; i++)
        keybuf[i] = 0;
    keybuf_len = 0;
}

//void digipot_debug(void)
//{
//    printf("\n");
//    printf("'");
//    for (int i=0; i<KEYBUF_SIZE; i++)
//        printf("%02X ", (uint8_t)keybuf[i]);
//    printf("' len=%d", keybuf_len);
//    printf("%s\n", ANSI_CLEAR_EOL);
//}

int digipot_screen_selected = 0;

static void digipot_action_down(void)
{
    if (digipot_screen_selected+1 < MAX_DIGIPOT_COUNT)
        digipot_screen_selected++;
    else
        digipot_screen_selected = 0;
    schedule_display_refresh();
}

static void digipot_action_up(void)
{
    if (digipot_screen_selected > 0)
        digipot_screen_selected--;
    else
        digipot_screen_selected = MAX_DIGIPOT_COUNT-1;
    schedule_display_refresh();
}

static void digipot_send_command(digipot_command_id_t id, uint32_t arg)
{
    CommandDigipots *cmd = osMailAlloc(mq_cmd_digipots_id, osWaitForever);
    if (!cmd) {
        goto err;
    }
    cmd->command_id = id;
    cmd->arg = arg;
    if (osOK != osMailPut(mq_cmd_digipots_id, cmd))
        goto err;
    return;
err:
    log_printf(LOG_WARNING, "pot command ignored: %d, %lu", id, arg);
}

static void digipot_action_reset(void)
{
    digipot_send_command(COMMAND_DIGIPOTS_RESET, digipot_screen_selected);
    schedule_display_refresh();
}

static void digipot_action_write(void)
{
    digipot_send_command(COMMAND_DIGIPOTS_WRITE, digipot_screen_selected);
    schedule_display_refresh();
}

static void digipot_action_plus(void)
{
    digipot_send_command(COMMAND_DIGIPOTS_DEC, digipot_screen_selected);
    schedule_display_refresh();
}

static void digipot_action_minus(void)
{
    digipot_send_command(COMMAND_DIGIPOTS_INC, digipot_screen_selected);
    schedule_display_refresh();
}

bool digipot_handle_escape_seq(const char *str)
{
    if (0 == strcmp(str, ESC_CODE_UP)) {
        digipot_action_up();
        return true;
    }
    if (0 == strcmp(str, ESC_CODE_DOWN)) {
        digipot_action_down();
        return true;
    }
    return false;
}

void digipot_screen_handle_key(const char ch)
{
    put_keybuf(ch);
    schedule_display_refresh();
    if (ch == 'u' || ch == 'U') {
        digipot_action_up();
    }
    if (ch == 'd' || ch == 'D') {
        digipot_action_down();
    }
    if (ch == '0') {
        digipot_action_reset();
    }
    if (ch == 'w' || ch == 'W') {
        digipot_action_write();
    }
    if (ch == '+') {
        digipot_action_plus();
    }
    if (ch == '-') {
        digipot_action_minus();
    }
    clear_keybuf();
}
