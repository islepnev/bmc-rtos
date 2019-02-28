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

#include "app_task_keyboard.h"

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "app_tasks.h"

osThreadId keyboardThreadId = NULL;
enum { keyboardThreadStackSize = 1000 };
static const uint32_t keyboardTaskLoopDelay = 10;

static void read_keys(void)
{
    int ch = getchar();
    if (ch == EOF)
        return;
    switch (ch) {
        case ' ':
        osSignalSet(displayThreadId, SIGNAL_REFRESH_DISPLAY);
        break;
    }
}

static void keyboardTask(void const *arg)
{
    (void) arg;
    while(1) {
        read_keys();
        osDelay(keyboardTaskLoopDelay);
    }
}

osThreadDef(keyboardThread, keyboardTask, osPriorityIdle,      1, keyboardThreadStackSize);

void create_task_keyboard(void)
{
    keyboardThreadId = osThreadCreate(osThread (keyboardThread), NULL);
    if (keyboardThreadId == NULL) {
        printf("Failed to create Keyboard thread\n");
    }
}
