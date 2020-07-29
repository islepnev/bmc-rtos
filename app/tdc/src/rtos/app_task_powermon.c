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
#include "app_task_digipot_impl.h"
#include "app_task_powermon_impl.h"
#include "app_task_sfpiic_impl.h"
#include "bsp.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "dev_thset.h"
#include "dev_thset_types.h"
#include "devicelist.h"
#include "ipmi_sensors.h"
#include "max31725/dev_max31725.h"
#include "max31725/dev_max31725_fsm.h"
#include "powermon/dev_powermon_types.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = 400 };
static const uint32_t powermonTaskLoopDelay = 10;

static BusInterface tdc64_max31725_1_bus_info = {
    .type = BUS_IIC,
    .bus_number = 4,
    .address = 0x50
};

static BusInterface tdc64_max31725_2_bus_info = {
    .type = BUS_IIC,
    .bus_number = 4,
    .address = 0x51
};

static BusInterface powermon_bus_info = {
    .type = BUS_IIC,
    .bus_number = 4,
    .address = 0
};

static Dev_powermon pm = {0};
static Dev_max31725 therm1 = {0};
static Dev_max31725 therm2 = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &pm.dev, &pm.priv, DEV_CLASS_POWERMON, powermon_bus_info);
#ifdef BOARD_TDC64
    create_device(parent, &therm1.dev, &therm1.priv, DEV_CLASS_THERM, tdc64_max31725_1_bus_info);
    create_device(parent, &therm2.dev, &therm2.priv, DEV_CLASS_THERM, tdc64_max31725_2_bus_info);
#endif
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
    Dev_thset *thset = get_dev_thset();
    Dev_thset zz = {0};
    *thset = zz;
    dev_thset_add(thset, "TDC-A");
    dev_thset_add(thset, "TDC-B");

    while (1)
    {
        // task_sfpiic_run(); // broken on tdc64
#ifdef BOARD_TDC64
        dev_max31725_run(&therm1);
        thset->sensors[0].value = therm1.priv.temp;
        thset->sensors[0].hdr.b.state = (therm1.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        dev_max31725_run(&therm2);
        thset->sensors[1].value = therm2.priv.temp;
        thset->sensors[1].hdr.b.state = (therm2.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        dev_thset_run(thset);
#endif
        task_digipot_run();
        task_powermon_run(&pm);
        sync_ipmi_sensors();

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
