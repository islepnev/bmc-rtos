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

#include "app_task_powermon.h"

#include "app_shared_data.h"
#include "app_task_powermon_impl.h"
#include "app_task_sfpiic_impl.h"
#include "bsp.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "devicelist.h"
#include "max31725/dev_max31725.h"
#include "max31725/dev_max31725_fsm.h"
#include "powermon/dev_powermon_types.h"
#include "tmp421/dev_tmp421.h"
#include "tmp421/dev_tmp421_fsm.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = 400 };
static const uint32_t powermonTaskLoopDelay = 10;

static BusInterface cru16_max31725_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x56
};

static BusInterface ttvxs_tmp421_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x1C
};

static BusInterface powermon_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0
};

static Dev_powermon pm = {0};
static Dev_max31725 therm1 = {0};
static Dev_tmp421 therm2 = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &pm.dev, &pm.priv, DEV_CLASS_POWERMON, powermon_bus_info);
    create_device(parent, &therm1.dev, &therm1.priv, DEV_CLASS_THERM, cru16_max31725_bus_info);
    create_device(parent, &therm2.dev, &therm2.priv, DEV_CLASS_THERM, ttvxs_tmp421_bus_info);
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
    while (1)
    {
        task_sfpiic_run();
        dev_max31725_run(&therm1);
        dev_tmp421_run(&therm2);
        task_powermon_run(&pm);
//        osEvent event = osSignalWait(SIGNAL_POWER_OFF, powermonTaskLoopDelay);
//        if (event.status == osEventSignal) {
//            pmState = PM_STATE_STANDBY;
//        }

        osDelay(powermonTaskLoopDelay);
    }
}

osThreadDef(powermon, start_task_powermon, osPriorityHigh,      1, powermonThreadStackSize);

void create_task_powermon(DeviceBase *parent)
{
    local_init(parent);
    powermonThreadId = osThreadCreate(osThread (powermon), NULL);
    if (powermonThreadId == NULL) {
        debug_print("Failed to create powermon thread\n");
    }
}
