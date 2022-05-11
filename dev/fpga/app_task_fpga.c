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

#include "app_task_fpga.h"

#include <assert.h>
#include <stdint.h>

#include "app_tasks.h"
#include "base/app_task_fpga_impl.h"
#include "bsp.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "fpga/dev_fpga_types.h"

osThreadId fpgaThreadId = NULL;
enum { fpgaThreadStackSize = threadStackSize + 130 };
static const uint32_t fpgaTaskLoopDelay = 50;

static BusInterface fpga_bus_info = {
    .type = BUS_SPI,
    .bus_number = SPI_BUS_INDEX_FPGA,
    .address = 0
};

static Dev_fpga d = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &d.dev, &d.priv, DEV_CLASS_FPGA, fpga_bus_info, "FPGA");
}

static void start_fpga_thread(void const *arg)
{
    (void) arg;

    for( ;; )
    {
        fpga_task_run(&d);
        osDelay(fpgaTaskLoopDelay);
    }
}

osThreadDef(fpga, start_fpga_thread, osPriorityNormal,  1, fpgaThreadStackSize);

void create_task_fpga(DeviceBase *parent)
{
    local_init(parent);
    fpgaThreadId = osThreadCreate(osThread (fpga), NULL);
    assert(fpgaThreadId);
}
