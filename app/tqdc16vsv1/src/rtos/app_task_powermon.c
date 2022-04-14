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

#include <assert.h>

#include "adt7301/dev_adt7301.h"
#include "adt7301/dev_adt7301_fsm.h"
#include "app_tasks.h"
#include "bsp.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "devicebase.h"
#include "devicelist.h"
#include "ipmi_sensors.h"
#include "log/log.h"
#include "thset/dev_thset.h"
#include "thset/dev_thset_types.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = threadStackSize + 120 };
static const uint32_t powermonTaskLoopDelay = 10;

static BusInterface tdc72_adt7301_bus_info = {
    .type = BUS_SPI,
    .bus_number = SPI_BUS_INDEX_ADT7301,
    .address = 0
};
static Dev_adt7301 therm[TQDC16VSV1_ADT7301_COUNT] = {0};

static Dev_thset thset = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &thset.dev, &thset.priv, DEV_CLASS_THSET, tdc72_adt7301_bus_info, "Thermometers");

    const char *therm_name[TQDC16VSV1_ADT7301_COUNT] = {"PLL", "Analog"};
    for (int i=0; i<TQDC16VSV1_ADT7301_COUNT; i++) {
        tdc72_adt7301_bus_info.address = i;
        create_device(&thset.dev, &therm[i].dev, &therm[i].priv, DEV_CLASS_ADT7301, tdc72_adt7301_bus_info, therm_name[i]);
    }
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
    for (int i=0; i<TQDC16VSV1_ADT7301_COUNT; i++) {
        dev_thset_add(&thset, therm[i].dev.name);
    }
    while (1) {
        const uint32_t start = osKernelSysTick();

        const bool power_on = true;
        for (int i=0; i<TQDC16VSV1_ADT7301_COUNT; i++) {
            dev_adt7301_run(&therm[i], power_on);
            thset.priv.sensors[i].value = therm[i].priv.temp;
            thset.priv.sensors[i].hdr.b.state = (therm[i].dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        }
        thset.priv.count = TQDC16VSV1_ADT7301_COUNT;
        dev_thset_run(&thset);
        sync_ipmi_sensors();

        const uint32_t finish = osKernelSysTick();
        const uint32_t elapsed = finish - start;
        if (elapsed > 100) {
            log_printf(LOG_WARNING, "%s task took over %.3f s", "powermon", elapsed / 1000.00);
        }

        osDelay(powermonTaskLoopDelay);
    }
}

osThreadDef(powermon, start_task_powermon, osPriorityHigh, 1, powermonThreadStackSize);

void create_task_powermon(DeviceBase *parent)
{
    local_init(parent);
    powermonThreadId = osThreadCreate(osThread (powermon), NULL);
    assert(powermonThreadId);
}
