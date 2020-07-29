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

#include "app_task_auxpll.h"

#include <stdint.h>

#include "cmsis_os.h"
#include "app_tasks.h"
#include "ad9516/dev_auxpll_types.h"
#include "ad9516/app_task_auxpll_impl.h"
#include "bus/bus_types.h"
#include "debug_helpers.h"

osThreadId auxpllThreadId = NULL;
enum { auxpllThreadStackSize = threadStackSize };
static const uint32_t auxpllTaskLoopDelay = 10;

static BusInterface auxpll_bus_info = {
    .type = BUS_SPI,
    .bus_number = 0, // FIXME
    .address = 0
};

static Dev_auxpll d = {0};

static void local_init(DeviceBase *parent) {
//    init_auxpll_setup(&d.priv.setup);
    create_device(parent, &d.dev, &d.priv, DEV_CLASS_AD9516, auxpll_bus_info, "Aux PLL");
}

static void auxpllTask(void const *arg)
{
    (void) arg;
    while(1) {
        auxpll_task_run(&d);
        osDelay(auxpllTaskLoopDelay);
    }
}

osThreadDef(auxpll, auxpllTask, osPriorityNormal,      1, auxpllThreadStackSize);

void create_task_auxpll(DeviceBase *parent)
{
    local_init(parent);
    auxpllThreadId = osThreadCreate(osThread (auxpll), NULL);
    if (auxpllThreadId == NULL) {
        debug_print("Failed to create auxpll thread\n");
    }
}
