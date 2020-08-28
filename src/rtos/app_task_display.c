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

#include "app_task_display.h"

#include <assert.h>

#include "base/app_task_display_impl.h"
#include "app_tasks.h"
#include "cmsis_os.h"

static const uint32_t displayTaskLoopDelay = 50;
osThreadId displayThreadId = NULL;
enum { displayThreadStackSize = threadStackSize + 550 };

static void start_display_task(void const *arg)
{
    (void) arg;
    display_task_init();
    while(1) {
        display_task_run();
        osDelay(displayTaskLoopDelay);
    }
}

osThreadDef(display, start_display_task, osPriorityLow,      1, displayThreadStackSize);

void create_task_display(void)
{
    displayThreadId = osThreadCreate(osThread (display), NULL);
    assert(displayThreadId);
}
