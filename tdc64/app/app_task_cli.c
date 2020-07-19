//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "app_task_cli.h"

#include <stdio.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "app_tasks.h"
#include "microrl.h"
#include "app_shared_data.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "commands_pot.h"

osThreadId cliThreadId = NULL;
enum { cliThreadStackSize = threadStackSize };

// print callback for microrl library
static void print (const char * str)
{
    fprintf (stdout, "%s", str);
}

// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (int argc, const char * const * argv)
{
    return 0;
}

void cycle_display_mode(void)
{
    switch (display_mode) {
    case DISPLAY_SUMMARY:
        display_mode = DISPLAY_LOG;
        break;
    case DISPLAY_LOG:
        display_mode = DISPLAY_POT;
        break;
    case DISPLAY_POT:
        display_mode = DISPLAY_PLL_DETAIL;
        break;
    case DISPLAY_PLL_DETAIL:
        display_mode = DISPLAY_TASKS;
        break;
    case DISPLAY_TASKS:
        display_mode = DISPLAY_SUMMARY;
        break;
    case DISPLAY_NONE:
        display_mode = DISPLAY_SUMMARY;
        break;
    default:
        break;
    }
}

static void screen_handle_key(char ch)
{
    switch (display_mode) {
    case DISPLAY_POT:
        pot_screen_handle_key(ch);
        break;
    default:
        break;
    }
}

static void cliTask(void const *arg)
{
    (void) arg;

    static microrl_t rl;
    static microrl_t * prl = &rl;
    microrl_init (prl, print);
    microrl_set_execute_callback (prl, execute);
    setvbuf(stdin, NULL, _IONBF, 0);
    for( ;; )
    {
        char ch = getchar();
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

osThreadDef(cli, cliTask,    osPriorityNormal, 1, cliThreadStackSize);

void create_task_cli(void)
{
    cliThreadId = osThreadCreate(osThread (cli), NULL);
    if (cliThreadId == NULL) {
        debug_print("Failed to create cli thread\n");
    }

}
