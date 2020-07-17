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
#include "app_shared_data.h"
#include "app_tasks.h"
#include "app_task_sfpiic_impl.h"
#include "app_task_digipot_impl.h"
#include "app_task_powermon_impl.h"
#include "ipmi_sensors.h"
#include "debug_helpers.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = 400 };
static const uint32_t powermonTaskLoopDelay = 10;

static void start_task_powermon( void const *arg)
{
    (void) arg;

    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

    while (1)
    {
        task_sfpiic_run();
        task_digipot_run();
        task_powermon_run();
        sync_ipmi_sensors();

//        osEvent event = osSignalWait(SIGNAL_POWER_OFF, powermonTaskLoopDelay);
//        if (event.status == osEventSignal) {
//            pmState = PM_STATE_STANDBY;
//        }

        osDelay(powermonTaskLoopDelay);
    }
}

osThreadDef(powermon, start_task_powermon, osPriorityHigh,      1, powermonThreadStackSize);

void create_task_powermon(void)
{
    powermonThreadId = osThreadCreate(osThread (powermon), NULL);
    if (powermonThreadId == NULL) {
        debug_print("Failed to create powermon thread\n");
    }
}
