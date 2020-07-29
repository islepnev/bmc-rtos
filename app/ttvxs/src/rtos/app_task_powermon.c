/*
**    Copyright 2019 Ilja Slepnev
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
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "thset/dev_thset.h"
#include "thset/dev_thset_types.h"
#include "devicebase.h"
#include "ipmi_sensors.h"
#include "max31725/dev_max31725.h"
#include "max31725/dev_max31725_fsm.h"
#include "powermon/dev_powermon_types.h"
#include "tmp421/dev_tmp421.h"
#include "tmp421/dev_tmp421_fsm.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = 400 };
static const uint32_t powermonTaskLoopDelay = 10;

static BusInterface ttvxs_max31725_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x52
};

static BusInterface ttvxs_tmp421_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x1C
};

static Dev_powermon pm = {0};
static Dev_max31725 therm1 = {0};
static Dev_tmp421 therm2 = {0};
static Dev_thset thset = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &pm.dev, &pm.priv, DEV_CLASS_POWERMON, null_bus_info, "Power Monitor");
    create_device(&pm.dev, &thset.dev, &thset.priv, DEV_CLASS_THSET, null_bus_info, "Thermometers");
    create_device(&thset.dev, &therm1.dev, &therm1.priv, DEV_CLASS_MAX31725, ttvxs_max31725_bus_info, "VCXO thermometer");
    create_device(&thset.dev, &therm2.dev, &therm2.priv, DEV_CLASS_TMP421, ttvxs_tmp421_bus_info, "FPGA, board thermometers");
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
    dev_thset_add(&thset, "VCXO");
    dev_thset_add(&thset, "FPGA");
    dev_thset_add(&thset, "Board");
    thset.priv.count = 3;
    while (1)
    {
        task_sfpiic_run();
        dev_max31725_run(&therm1);
        dev_tmp421_run(&therm2);
        thset.priv.sensors[0].value = therm1.priv.temp;
        thset.priv.sensors[0].hdr.b.state = (therm1.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        thset.priv.sensors[1].value = therm2.priv.temp;
        thset.priv.sensors[1].hdr.b.state = (therm2.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        thset.priv.sensors[2].value = therm2.priv.temp_internal;
        thset.priv.sensors[2].hdr.b.state = (therm2.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        dev_thset_run(&thset);
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
