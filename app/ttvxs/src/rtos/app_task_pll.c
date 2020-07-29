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
#include "app_task_clkmux_impl.h"
#include "app_tasks.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "eeprom_config/dev_eeprom_config_fsm.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux_types.h"

osThreadId pllThreadId = NULL;
enum { pllThreadStackSize = threadStackSize };
static const uint32_t pllTaskLoopDelay = 50;

static BusInterface pll_bus_info = {
    .type = BUS_IIC,
    .bus_number = 4,
    .address = 0x4A
};

static BusInterface clkmux_bus_info = {
    .type = BUS_IIC,
    .bus_number = 4,
    .address = 0x20
};

// mezzanine eeprom
static BusInterface eeprom_config_bus_info = {
    .type = BUS_IIC,
    .bus_number = 3,
    .address = 0x50
};

static Dev_ad9545 pll = {0};
static Dev_eeprom_config eeprom = {0};
static Dev_ttvxs_clkmux clkmux = {0};

static void local_init(DeviceBase *parent)
{
    init_ad9545_setup(&pll.priv.setup);
    create_device(parent, &pll.dev, &pll.priv, DEV_CLASS_AD9545, pll_bus_info, "Main PLL");
    create_device(parent, &clkmux.dev, &clkmux.priv, DEV_CLASS_CLKMUX, clkmux_bus_info, "CLock Mux");
    create_device(parent, &eeprom.dev, &eeprom.priv, DEV_CLASS_EEPROM, eeprom_config_bus_info, "EEPROM-Config");
}

static void pllTask(void const *arg)
{
    (void) arg;

    ad9545_gpio_init(&pll.dev.bus);

    while(1) {
        dev_eeprom_config_run(&eeprom);
        task_clkmux_run(&clkmux);
        dev_ad9545_run(&pll);
        osDelay(pllTaskLoopDelay);
    }
}

osThreadDef(pll, pllTask, osPriorityBelowNormal,      1, pllThreadStackSize);

void create_task_pll(DeviceBase *parent)
{
    local_init(parent);
    pllThreadId = osThreadCreate(osThread (pll), NULL);
    if (pllThreadId == NULL) {
        debug_print("Failed to create pll thread\n");
    }
}
