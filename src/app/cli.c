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

#include "cli.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "app_task_cli_impl.h"
#include "debug_helpers.h"
#include "display.h"
#include "logbuffer.h"
#include "microrl.h"

// print callback for microrl library
static void print (const char * str)
{
    fprintf (stdout, "%s", str);
}

// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
static int execute (int argc, const char * const * argv)
{
    return 0;
}

static int new_screen_width = 0;
static int new_screen_height = 0;
static int prev_screen_width = 0;
static int prev_screen_height = 0;

static void handle_escape_screen_pos(uint16_t row, uint16_t col)
{
    if (row >= 1 && row <= 999 && col >= 1 && col <= 999) {
        prev_screen_width = new_screen_width;
        prev_screen_height = new_screen_height;
        new_screen_width = col;
        new_screen_height = row;
        if (prev_screen_width == new_screen_width &&
            prev_screen_height == new_screen_height) {
            if (screen_width != new_screen_width || screen_height != new_screen_height) {
                screen_width = new_screen_width;
                screen_height = new_screen_height;
                schedule_display_repaint();
            }
        }
    }
}

static bool handle_escape_seq(const char *str)
{
    size_t len = strlen(str);
    // [55;165R
    if (len >= 5 && str[0] == '[' && str[len-1] == 'R') {
        char s1[4] = {0};
        char s2[4] = {0};
        size_t i = 1, p1=0, p2=0;
        while (i < len-1) {
            if (! (str[i] >= '0' && str[i] <= '9'))
                break;
            s1[p1++] = str[i];
            i++;
        }
        if (str[i++] != ';')
            return false;
        while (i < len-1) {
            if (! (str[i] >= '0' && str[i] <= '9'))
                break;
            s2[p2++] = str[i];
            i++;
        }
        if (str[i] != 'R')
            return false;
        int row = atoi(s1);
        int col = atoi(s2);
        handle_escape_screen_pos(row, col);
        return true;
    }
    return false;
}


static char escbuf[32] = {0};
static size_t escpos = 0;

static bool handle_escape_char(char ch)
{
    if (escpos >= sizeof (escbuf)-1) {
        log_printf(LOG_DEBUG, "Too long esc sequence: %s", escbuf);
        return false;
    }
    escbuf[escpos++] = ch;
    escbuf[escpos] = '\0';
    // log_printf(LOG_DEBUG, "esc input[%d]: %02X", escpos, ch);
    bool is_number = (ch >= '0' && ch <= '9');
    bool complete = ! (is_number || ch == ';' || ch == '[');
    if (complete) {
        escbuf[sizeof (escbuf)-1] = '\0';
        // log_printf(LOG_DEBUG, "Complete esc sequence: %s", escbuf);
        if (!handle_escape_seq(escbuf)) {
            if (!app_handle_escape_seq(escbuf)) {
//                print_goto(screen_height-1, 1);
//                printf("Unhandled ESC sequence: %s%s", escbuf, ANSI_CLEAR_EOL);
            }
        }
        escpos = 0;
    }
    return ! complete;
}

void cliTask(void const *arg)
{
    (void) arg;

    static microrl_t rl;
    static microrl_t * prl = &rl;
    microrl_init (prl, print);
    microrl_set_execute_callback (prl, execute);
    setvbuf(stdin, NULL, _IONBF, 0);
    static bool esc = false;
    for( ;; )
    {
        char ch = getchar();
        if (ch == '\x1B') {
            esc = true;
            escpos = 0;
            continue;
        }
        if (esc) {
            esc = handle_escape_char(ch);
            continue;
        }
        switch(ch) {
        case ' ':
        case '\r':
        case '\n':
            cycle_display_mode();
            break;
        case 'p':
        case 'P':
            enable_power = !enable_power;
            if (enable_power)
                log_put(LOG_INFO, "Received command power ON");
            else
                log_put(LOG_INFO, "Received command power OFF");
            break;
        default:
            screen_handle_key(ch);
            break;
        }
//        microrl_insert_char (prl, ch);
    }
}
