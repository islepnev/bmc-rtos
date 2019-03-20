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
        case 'p':
        case 'P':
            enable_power = !enable_power;
            if (enable_power)
                log_put(LOG_INFO, "Received command power ON");
            else
                log_put(LOG_INFO, "Received command power OFF");
            break;
        default:
            enable_stats_display = !enable_stats_display;
            break;
        }
//        microrl_insert_char (prl, ch);
    }
}

osThreadDef(cli, cliTask,    osPriorityLow, 1, cliThreadStackSize);

void create_task_cli(void)
{
    osThreadId threadId = osThreadCreate(osThread (cli), NULL);
    if (threadId == NULL) {
        printf("Failed to create cli thread\n");
    }

}
