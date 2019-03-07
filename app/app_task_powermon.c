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

#include "app_task_powermon.h"

#include "cmsis_os.h"

#include "dev_powermon.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "app_tasks.h"
#include "app_task_powermon_impl.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = threadStackSize };
static const uint32_t powermonTaskLoopDelay = 2;

PmState getPmState(void)
{
    return pmState;
}

Dev_powermon getPmData(void)
{
    return dev.pm;
}

uint32_t getPmLoopCount(void)
{
    return pmLoopCount;
}

static void prvPowermonTask( void const *arg)
{
    (void) arg;

    while (1)
    {
        powermon_task();
//        osEvent event = osSignalWait(SIGNAL_POWER_OFF, powermonTaskLoopDelay);
//        if (event.status == osEventSignal) {
//            pmState = PM_STATE_STANDBY;
//        }

        osDelay(powermonTaskLoopDelay);
    }
}

osThreadDef(powermonThread, prvPowermonTask, osPriorityHigh,      1, powermonThreadStackSize);

void create_task_powermon(void)
{
    powermonThreadId = osThreadCreate(osThread (powermonThread), NULL);
    if (powermonThreadId == NULL) {
        printf("Failed to create Powermon thread\n");
    }
}
