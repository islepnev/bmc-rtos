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

#include <stdint.h>

#include "ad9545/ad9545.h"
#include "ad9545/dev_ad9545.h"
#include "ad9545/dev_ad9545_fsm.h"
#include "app_task_eeprom_config_impl.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "eeprom_config/dev_eeprom_config.h"

osThreadId pllThreadId = NULL;
enum { pllThreadStackSize = 400 };
static const uint32_t pllTaskLoopDelay = 30;

static BusInterface pll_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x4A
};

static BusInterface eeprom_config_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x50
};

Dev_ad9545 d = {0};

static void local_init(void) {
    init_ad9545_setup(&d.priv.setup);
    create_device(&d.dev, &d.priv, DEV_CLASS_PLL, pll_bus_info);
}

static void pllTask(void const *arg)
{
    (void) arg;

    ad9545_gpio_init(&d.dev.bus);

    dev_eeprom_config_init(&eeprom_config_bus_info);
    while(1) {
        task_eeprom_config_run();
        dev_ad9545_run(&d);
        osDelay(pllTaskLoopDelay);
    }
}

osThreadDef(pll, pllTask, osPriorityBelowNormal,      1, pllThreadStackSize);

void create_task_pll(void)
{
    local_init();
    pllThreadId = osThreadCreate(osThread (pll), NULL);
    if (pllThreadId == NULL) {
        debug_print("Failed to create pll thread\n");
    }
}
