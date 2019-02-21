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

#include "app_tasks.h"
#include "app_task_heartbeat.h"
#include "app_task_main.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainAPPMAIN_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

void create_tasks(void)
{
    printf("Creating tasks...");
    fflush(stdout);
    create_task_heartbeat(mainQUEUE_SEND_TASK_PRIORITY, mainQUEUE_RECEIVE_TASK_PRIORITY);
    create_task_main(mainAPPMAIN_TASK_PRIORITY);
    printf("Ok\n");
    fflush(stdout);
}
