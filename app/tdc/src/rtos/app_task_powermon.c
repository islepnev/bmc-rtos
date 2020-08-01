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

#include "adt7301/dev_adt7301.h"
#include "adt7301/dev_adt7301_fsm.h"
#include "app_shared_data.h"
#include "app_task_powermon_impl.h"
#include "bsp.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "devicebase.h"
#include "devicelist.h"
#include "digipot/dev_digipot_fsm.h"
#include "digipot/dev_digipot_types.h"
#include "digipot/dev_digipot_types.h"
#include "init_digipot_subdevices.h"
#include "ipmi_sensors.h"
#include "max31725/dev_max31725.h"
#include "max31725/dev_max31725_fsm.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_types.h"
#include "sfpiic/dev_sfpiic_fsm.h"
#include "sfpiic/dev_sfpiic_fsm.h"
#include "sfpiic/dev_sfpiic_types.h"
#include "thset/dev_thset.h"
#include "thset/dev_thset_types.h"

osThreadId powermonThreadId = NULL;
enum { powermonThreadStackSize = 400 };
static const uint32_t powermonTaskLoopDelay = 10;

static Dev_powermon pm = {0};
#ifdef BOARD_TDC64
static BusInterface tdc64_max31725_bus_info[TDC64_MAX31725_COUNT] = {
    {
        .type = BUS_IIC,
        .bus_number = 4,
        .address = 0x50
    }, {
        .type = BUS_IIC,
        .bus_number = 4,
        .address = 0x51
    }
};
static Dev_max31725 therm[TDC64_MAX31725_COUNT] = {0};
#endif
#ifdef BOARD_TDC72
static BusInterface tdc72_adt7301_bus_info = {
    .type = BUS_SPI,
    .bus_number = 4,
    .address = 0
};
static Dev_adt7301 therm[TDC72_ADT7301_COUNT] = {0};
#endif
static Dev_digipots digipots = {0};
static Dev_thset thset = {0};
static Dev_sfpiic sfpiic = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &pm.dev, &pm.priv, DEV_CLASS_POWERMON, null_bus_info, "Power Monitor");
    create_device(&pm.dev, &thset.dev, &thset.priv, DEV_CLASS_THSET, null_bus_info, "Thermometers");

#ifdef BOARD_TDC64
    const char *therm_name[TDC64_MAX31725_COUNT] = {"TDC-A", "TDC-B"};
    for (int i=0; i<TDC64_MAX31725_COUNT; i++) {
        create_device(&thset.dev, &therm[i].dev, &therm[i].priv, DEV_CLASS_MAX31725, tdc64_max31725_bus_info[i], therm_name[i]);
    }
#endif
#ifdef BOARD_TDC72
    const char *therm_name[TDC72_ADT7301_COUNT] = {"PLL", "TDC-A", "TDC-B", "TDC-C"};
    for (int i=0; i<TDC72_ADT7301_COUNT; i++) {
        tdc72_adt7301_bus_info.address = i;
        create_device(&thset.dev, &therm[i].dev, &therm[i].priv, DEV_CLASS_ADT7301, tdc72_adt7301_bus_info, therm_name[i]);
    }
#endif
    create_device(&pm.dev, &digipots.dev, &digipots.priv, DEV_CLASS_DIGIPOTS, null_bus_info, "DigiPots");
    create_digipots_subdevices(&digipots);
    create_sensor_subdevices(&pm);
    create_device(parent, &sfpiic.dev, &sfpiic.priv, DEV_CLASS_SFPIIC, null_bus_info, "SFP IIC");
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
#ifdef BOARD_TDC72
    for (int i=0; i<TDC72_ADT7301_COUNT; i++) {
        dev_thset_add(&thset, therm[i].dev.name);
    }
#endif
#ifdef BOARD_TDC64
    for (int i=0; i<TDC64_MAX31725_COUNT; i++) {
        dev_thset_add(&thset, therm[i].dev.name);
    }
#endif
    while (1)
    {
#ifndef BOARD_TDC64 // issue #669
        sfpiic_switch_enable(true);
        task_sfpiic_run(&sfpiic);
#endif
        sfpiic_switch_enable(false);
#ifdef BOARD_TDC72
        for (int i=0; i<TDC72_ADT7301_COUNT; i++) {
            bool power_on = enable_power && system_power_present;
            dev_adt7301_run(&therm[i], power_on);
            thset.priv.sensors[i].value = therm[i].priv.temp;
            thset.priv.sensors[i].hdr.b.state = (therm[i].dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        }
        thset.priv.count = TDC72_ADT7301_COUNT;
        dev_thset_run(&thset);
#endif
#ifdef BOARD_TDC64
        for (int i=0; i<TDC64_MAX31725_COUNT; i++) {
            bool power_on = enable_power && system_power_present;
            dev_max31725_run(&therm[i], power_on);
            thset.priv.sensors[i].value = therm[i].priv.temp;
            thset.priv.sensors[i].hdr.b.state = (therm[i].dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        }
        thset.priv.count = TDC64_MAX31725_COUNT;
        dev_thset_run(&thset);
#endif
        dev_digipot_run(&digipots);
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
