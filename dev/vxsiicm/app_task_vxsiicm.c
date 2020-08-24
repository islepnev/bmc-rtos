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

#include "app_task_vxsiicm.h"

#include <assert.h>

#include "app_tasks.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "vxsiicm/dev_vxsiicm.h"
#include "vxsiicm/dev_vxsiicm_fsm.h"
#include "vxsiicm/dev_vxsiicm_types.h"

osThreadId vxsiicThreadId = NULL;
enum { vxsiicThreadStackSize = threadStackSize + 170 };

static const uint32_t vxsiicTaskLoopDelay = 10;

static Dev_vxsiicm vxsiicm = {0};
static BusInterface vxsiicm_bus_info = {
    .type = BUS_IIC,
    .bus_number = 1,
    .address = 0
};

static void local_init(DeviceBase *parent) {
    //    init_auxpll_setup(&d.priv.setup);
    create_device(parent, &vxsiicm.dev, &vxsiicm.priv, DEV_CLASS_VXSIICM, vxsiicm_bus_info, "VXS IIC Master");
}

static void start_thread_vxsiicm( void const *arg)
{
    (void) arg;

    while (1)
    {
        dev_vxsiicm_run(&vxsiicm);
        osDelay(vxsiicTaskLoopDelay);
    }
}

osThreadDef(vxsiicm, start_thread_vxsiicm,    osPriorityNormal, 1, vxsiicThreadStackSize);

void create_task_vxsiicm(DeviceBase *parent)
{
    local_init(parent);
    vxsiicThreadId = osThreadCreate(osThread (vxsiicm), NULL);
    assert(vxsiicThreadId);
}
