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

#include "app_task_pll.h"

#include <assert.h>
#include <stdint.h>

#include "ad9548/ad9548.h"
#include "ad9548/ad9548_setup.h"
#include "ad9548/dev_ad9548.h"
#include "ad9548/dev_ad9548_fsm.h"
#include "app_shared_data.h"
#include "app_tasks.h"
#include "bsp.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "log/log.h"

osThreadId pllThreadId = NULL;
enum { pllThreadStackSize = threadStackSize + 80 };
static const uint32_t pllTaskLoopPeriod = 50;

static BusInterface pll_bus_info = {
    .type = BUS_SPI,
    .bus_number = SPI_BUS_INDEX_AD9548,
    .address = 0
};


static Dev_ad9548 pll = {0};

static void local_init(DeviceBase *parent)
{
    init_ad9548_setup(&pll.priv.setup);
    create_device(parent, &pll.dev, &pll.priv, DEV_CLASS_AD9548, pll_bus_info, "Main PLL");
}

static void run(void)
{
    const bool power_on = enable_power && system_power_present;
    dev_ad9548_run(&pll, power_on);
    main_clock_ready = ad9548_running(&pll);
}

static void pllTask(void const *arg)
{
    (void) arg;

    ad9548_gpio_init(&pll.dev.bus);

    while(1) {
        const uint32_t start = osKernelSysTick();
        run();
        const uint32_t finish = osKernelSysTick();
        const uint32_t elapsed = finish - start;
        if (elapsed > pllTaskLoopPeriod) {
            log_printf(LOG_NOTICE, "%s task time %.3f s", "pll", elapsed / 1000.00);
        }
        const int32_t delay = pllTaskLoopPeriod - elapsed;
        osDelay(delay > 1 ? delay : 1);
    }
}

osThreadDef(pll, pllTask, osPriorityBelowNormal, 1, pllThreadStackSize);

void create_task_pll(DeviceBase *parent)
{
    local_init(parent);
    pllThreadId = osThreadCreate(osThread (pll), NULL);
    assert(pllThreadId);
}
