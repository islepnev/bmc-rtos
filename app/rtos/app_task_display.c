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

#include "app_task_display.h"
#include "app_task_display_impl.h"
#include "cmsis_os.h"
#include "app_tasks.h"
#include "debug_helpers.h"

static const uint32_t displayTaskLoopDelay = 500;
osThreadId displayThreadId = NULL;
enum { displayThreadStackSize = 1000 };

static void start_display_task(void const *arg)
{
    (void) arg;
    display_task_init();
    while(1) {
        display_task_run();
        osDelay(displayTaskLoopDelay);
    }
}

osThreadDef(display, start_display_task, osPriorityIdle,      1, displayThreadStackSize);

void create_task_display(void)
{
    displayThreadId = osThreadCreate(osThread (display), NULL);
    if (displayThreadId == NULL) {
        debug_print("Failed to create display thread\n");
    }
}
