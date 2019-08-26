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
#include "commands_pot.h"

#include <string.h>

#include "commands.h"
#include "dev_pot.h"
#include "logbuffer.h"
#include "app_shared_data.h"

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

//void pot_debug(void)
//{
//    printf("\n");
//    printf("'");
//    for (int i=0; i<KEYBUF_SIZE; i++)
//        printf("%02X ", (uint8_t)keybuf[i]);
//    printf("' len=%d", keybuf_len);
//    printf("%s\n", ANSI_CLEAR_EOL);
//}

int pot_screen_selected = 0;

static void pot_action_down(void)
{
    if (pot_screen_selected+1 < DEV_POT_COUNT)
        pot_screen_selected++;
    else
        pot_screen_selected = 0;
    schedule_display_refresh();
}

static void pot_action_up(void)
{
    if (pot_screen_selected > 0)
        pot_screen_selected--;
    else
        pot_screen_selected = DEV_POT_COUNT-1;
    schedule_display_refresh();
}

static void pot_send_command(command_id_t id, uint32_t arg)
{
    CommandPots *cmd = osMailAlloc(mq_cmd_pots_id, osWaitForever);
    if (!cmd) {
        goto err;
    }
    cmd->command_id = id;
    cmd->arg = arg;
    if (osOK != osMailPut(mq_cmd_pots_id, cmd))
        goto err;
    return;
err:
    log_printf(LOG_WARNING, "pot command ignored: %d, %lu", id, arg);
}

static void pot_action_reset(void)
{
    pot_send_command(COMMAND_POTS_RESET, pot_screen_selected);
    schedule_display_refresh();
}

static void pot_action_write(void)
{
    pot_send_command(COMMAND_POTS_WRITE, pot_screen_selected);
    schedule_display_refresh();
}

static void pot_action_plus(void)
{
    pot_send_command(COMMAND_POTS_DEC, pot_screen_selected);
    schedule_display_refresh();
}

static void pot_action_minus(void)
{
    pot_send_command(COMMAND_POTS_INC, pot_screen_selected);
    schedule_display_refresh();
}

void pot_screen_handle_key(const char ch)
{
    if (keybuf_len == 0 && ch == '\x1B') {
        put_keybuf(ch);
        schedule_display_refresh();
        return;
    }
    if (keybuf_len == 1 && keybuf[0] == '\x1B' && ch == '[') {
        put_keybuf(ch);
        schedule_display_refresh();
        return;
    }
    put_keybuf(ch);
    schedule_display_refresh();
    if (0 == strncmp(keybuf, "\x1B[A", 3) || ch == 'u' || ch == 'U') {
        pot_action_up();
    }
    if (0 == strncmp(keybuf, "\x1B[B", 3) || ch == 'd' || ch == 'D') {
        pot_action_down();
    }
    if (ch == '0') {
        pot_action_reset();
    }
    if (ch == 'w' || ch == 'W') {
        pot_action_write();
    }
    if (ch == '+') {
        pot_action_plus();
    }
    if (ch == '-') {
        pot_action_minus();
    }
    clear_keybuf();
}
