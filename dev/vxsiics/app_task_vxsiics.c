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

#include "app_task_vxsiics.h"

#include <assert.h>

#include "app_tasks.h"
#include "bus/i2c_slave_driver.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "vxsiics/dev_vxsiics_fsm.h"
#include "vxsiics/dev_vxsiics_types.h"

osThreadId vxsiicsThreadId = NULL;
enum { vxsiicThreadStackSize = threadStackSize + 180 };

static const uint32_t vxsiicTaskLoopDelay = 10;

static Dev_vxsiics vxsiics = {0};
static BusInterface vxsiics_bus_info = {
    .type = BUS_IIC,
    .bus_number = 1,
    .address = VXSIIC_BUS_ADDRESS
};

static void local_init(DeviceBase *parent) {
    i2c_slave_driver_init();
    create_device(parent, &vxsiics.dev, &vxsiics.priv, DEV_CLASS_VXSIICS, vxsiics_bus_info, "VXS IIC Slave");
}

static void start_thread_vxsiics( void const *arg)
{
    (void) arg;

    while (1)
    {
        dev_vxsiics_run(&vxsiics);
        osDelay(vxsiicTaskLoopDelay);
    }
}

osThreadDef(vxsiics, start_thread_vxsiics,    osPriorityLow, 1, vxsiicThreadStackSize);

void create_task_vxsiics(DeviceBase *parent)
{
    local_init(parent);
    vxsiicsThreadId = osThreadCreate(osThread (vxsiics), NULL);
    assert(vxsiicsThreadId);
}
