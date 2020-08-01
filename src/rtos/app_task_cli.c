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

#include "app_tasks.h"
#include "cli.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "log/log.h"

osThreadId cliThreadId = NULL;
enum { cliThreadStackSize = threadStackSize };

osThreadDef(cli, cliTask,    osPriorityNormal, 1, cliThreadStackSize);

void create_task_cli(void)
{
    cliThreadId = osThreadCreate(osThread (cli), NULL);
    if (cliThreadId == NULL) {
        debug_print("Failed to create cli thread\n");
    }

}
