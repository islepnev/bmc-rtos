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

#include "app_task_main.h"

#include <assert.h>

#include "base/app_task_main_impl.h"
#include "app_tasks.h"
#include "cmsis_os.h"

enum { mainThreadStackSize = threadStackSize };
static const uint32_t mainTaskLoopDelay = 10;

static void start_thread_main(void const *arg)
{
    (void) arg;
    task_main_init();
    while (1)
    {
        task_main_run();
        osDelay(mainTaskLoopDelay);
    }
}

osThreadDef(main, start_thread_main, osPriorityNormal,      1, mainThreadStackSize);

void create_task_main(void)
{
    osThreadId mainThreadId = osThreadCreate(osThread (main), NULL);
    assert(mainThreadId);
}
