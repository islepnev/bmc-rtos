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
#include <string.h>

#include "app_shared_data.h"
#include "app_task_powermon_impl.h"
#include "bsp.h"
#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "devicebase.h"
#include "devicelist.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "eeprom_config/dev_eeprom_config_fsm.h"
#include "ipmi_sensors.h"
#include "log/log.h"
#include "max31725/dev_max31725.h"
#include "max31725/dev_max31725_fsm.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_types.h"
#include "sfpiic/dev_sfpiic_fsm.h"
#include "sfpiic/dev_sfpiic_types.h"
#include "thset/dev_thset.h"
#include "thset/dev_thset_types.h"
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

static BusInterface config_eeprom_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0x57
};

static BusInterface ttvxs_smbus_bus_info = {
    .type = BUS_IIC,
    .bus_number = 2,
    .address = 0
};

static Dev_powermon pm = {0};
static Dev_max31725 therm1 = {0};
static Dev_tmp421 therm2 = {0};
static Dev_thset thset = {0};
static Dev_sfpiic sfpiic = {0};
static Dev_eeprom_config eeprom = {0};

static void local_init(DeviceBase *parent)
{
    create_device(parent, &pm.dev, &pm.priv, DEV_CLASS_POWERMON, ttvxs_smbus_bus_info, "Power Monitor");
    create_device(&pm.dev, &thset.dev, &thset.priv, DEV_CLASS_THSET, ttvxs_smbus_bus_info, "Thermometers");
    create_device(&thset.dev, &therm1.dev, &therm1.priv, DEV_CLASS_MAX31725, ttvxs_max31725_bus_info, "VCXO temperature");
    create_device(&thset.dev, &therm2.dev, &therm2.priv, DEV_CLASS_TMP421, ttvxs_tmp421_bus_info, "FPGA, board temperatures");
    create_sensor_subdevices(&pm);

    create_device(parent, &sfpiic.dev, &sfpiic.priv, DEV_CLASS_SFPIIC, ttvxs_smbus_bus_info, "SFP IIC");
    sfpiic.priv.portCount = 4;
    sfpiic.priv.portIndex[0] = 3;
    sfpiic.priv.portIndex[1] = 2;
    sfpiic.priv.portIndex[2] = 1;
    sfpiic.priv.portIndex[3] = 0;
    strncpy(sfpiic.priv.portName[0], "SFP-1", SFPIIC_PORT_NAME_LEN);
    strncpy(sfpiic.priv.portName[1], "SFP-2", SFPIIC_PORT_NAME_LEN);
    strncpy(sfpiic.priv.portName[2], "SFP-3", SFPIIC_PORT_NAME_LEN);
    strncpy(sfpiic.priv.portName[3], "SFP-4", SFPIIC_PORT_NAME_LEN);

    create_device(parent, &eeprom.dev, &eeprom.priv, DEV_CLASS_EEPROM, config_eeprom_bus_info, "Board config");
}

static void start_task_powermon( void const *arg)
{
    (void) arg;
    dev_thset_add(&thset, "Board");
    dev_thset_add(&thset, "FPGA");
    dev_thset_add(&thset, "VCXO");
    thset.priv.count = 3;
    while (1)
    {
        const uint32_t start = osKernelSysTick();

        const bool power_on = enable_power && system_power_present;
        sfpiic_switch_enable(false);
        dev_eeprom_config_run(&eeprom);
        sfpiic_switch_enable(power_on);
        task_sfpiic_run(&sfpiic, power_on);
        sfpiic_switch_enable(false);
        dev_max31725_run(&therm1, power_on);
        dev_tmp421_run(&therm2);
        thset.priv.sensors[0].value = therm2.priv.temp_internal;
        thset.priv.sensors[0].hdr.b.state = (therm2.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        thset.priv.sensors[1].value = therm2.priv.temp;
        thset.priv.sensors[1].hdr.b.state = (therm2.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        thset.priv.sensors[2].value = therm1.priv.temp;
        thset.priv.sensors[2].hdr.b.state = (therm1.dev.device_status == DEVICE_NORMAL) ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        dev_thset_run(&thset);
        task_powermon_run(&pm);
        sync_ipmi_sensors();

//        osEvent event = osSignalWait(SIGNAL_POWER_OFF, powermonTaskLoopDelay);
//        if (event.status == osEventSignal) {
//            pmState = PM_STATE_STANDBY;
//        }
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
