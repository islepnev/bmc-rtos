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

// CRU16 board specific functions

#include "bsp_powermon.h"

#include "bsp_pin_defs.h"
#include "cmsis_os.h"
#include "bsp_sensors_config.h"
#include "gpio.h"
#include "log/log.h"
#include "powermon/dev_pm_sensors.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon_types.h"
#include "stm32f7xx_hal_gpio.h"

int monIsOn(const pm_switches sw, SensorIndex index)
{
    switch(index) {
    case SENSOR_VME_3V3: return 1;
    case SENSOR_5VPC: return 1;
    case SENSOR_5V: return sw[PSW_5V];
    case SENSOR_VME_5V: return 1;
    case SENSOR_1V5: return sw[PSW_5V] && sw[PSW_1V5];
    case SENSOR_3V3: return sw[PSW_3V3];
    case SENSOR_FPGA_CORE_1V0: return sw[PSW_5V] && sw[PSW_1V0_CORE];
    case SENSOR_FPGA_MGT_1V0: return sw[PSW_5V] && sw[PSW_1V0_MGT];
    case SENSOR_FPGA_MGT_1V2: return sw[PSW_5V] && sw[PSW_1V2_MGT];
    case SENSOR_MCB_4V5: return 1;
    case SENSOR_FPGA_1V8: return sw[PSW_3V3];
    case SENSOR_CLK_2V5: return sw[PSW_3V3];
    case SENSOR_CLK_3V3: return sw[PSW_5V];
    case SENSOR_MCB_3V3: return 1;
    }
    return 0;
}

bool readLiveInsertPin(void)
{
    bool state = true; // TODO: read_gpio_pin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (false == state);
}

void init_power_switches(pm_switches sw)
{
    for (int i=0; i<POWER_SWITCH_COUNT; i++)
        sw[i] = true;
}

void read_power_switches_state(pm_switches sw_state)
{
    sw_state[PSW_1V0_CORE] = read_gpio_pin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin);
    sw_state[PSW_1V0_MGT]  = read_gpio_pin(ON_1V0_MGT_GPIO_Port,  ON_1V0_MGT_Pin);
    sw_state[PSW_1V2_MGT]  = read_gpio_pin(ON_1V2_MGT_GPIO_Port,  ON_1V2_MGT_Pin);
    sw_state[PSW_1V5]      = read_gpio_pin(ON_1V5_GPIO_Port,      ON_1V5_Pin);
    sw_state[PSW_3V3]      = read_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin);
    sw_state[PSW_2V5_CLK]  = read_gpio_pin(ON_2V5_CLK_GPIO_Port,  ON_2V5_CLK_Pin);
    sw_state[PSW_5V]       = read_gpio_pin(ON_5V_VXS_GPIO_Port,   ON_5V_VXS_Pin);
}

void write_power_switches(pm_switches sw)
{
    write_gpio_pin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin, sw[PSW_1V0_CORE]);
    write_gpio_pin(ON_1V0_MGT_GPIO_Port,  ON_1V0_MGT_Pin,  sw[PSW_1V0_MGT]);
    write_gpio_pin(ON_1V2_MGT_GPIO_Port,  ON_1V2_MGT_Pin,  sw[PSW_1V2_MGT]);
    write_gpio_pin(ON_1V5_GPIO_Port,      ON_1V5_Pin,      sw[PSW_1V5]);
    write_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin,      sw[PSW_3V3]);
    write_gpio_pin(ON_2V5_CLK_GPIO_Port,  ON_2V5_CLK_Pin,  sw[PSW_2V5_CLK]);
    write_gpio_pin(ON_5V_VXS_GPIO_Port,   ON_5V_VXS_Pin,   sw[PSW_5V]);
}

void init_pgood(pm_pgoods pgood)
{
    for (int i=0; i<POWER_GOOD_COUNT; i++)
        pgood[i] = false;
}

static int read_pgood_1v0_core(void)
{
    return read_gpio_pin(PGOOD_1V0_CORE_GPIO_Port, PGOOD_1V0_CORE_Pin);
}

static int read_pgood_1v0_mgt(void)
{
    return read_gpio_pin(PGOOD_1V0_MGT_GPIO_Port, PGOOD_1V0_MGT_Pin);
}

static int read_pgood_1v2_mgt(void)
{
    return read_gpio_pin(PGOOD_1V2_MGT_GPIO_Port, PGOOD_1V2_MGT_Pin);
}

static int read_pgood_1v5(void)
{
    return read_gpio_pin(PGOOD_1V5_GPIO_Port, PGOOD_1V5_Pin);
}

static int read_pgood_vtt(void)
{
    return read_gpio_pin(PGOOD_VTT_GPIO_Port, PGOOD_VTT_Pin);
}

void pm_read_pgood(pm_pgoods pgood)
{
    pgood[PGOOD_1V0_CORE] = read_pgood_1v0_core();
    pgood[PGOOD_1V0_MGT]  = read_pgood_1v0_mgt();
    pgood[PGOOD_1V2_MGT]  = read_pgood_1v2_mgt();
    pgood[PGOOD_1V5]      = read_pgood_1v5();
    pgood[PGOOD_VTT]      = read_pgood_vtt();
}

bool get_all_pgood(const pm_pgoods pgood)
{
    for (int i=0; i<POWER_GOOD_COUNT; i++)
        if (!pgood[i])
            return false;
    return true;
}

bool get_input_power_valid(const pm_sensors_arr *sensors)
{
    return pm_sensor_isValid(&sensors->arr[SENSOR_VME_5V]);
}

bool get_input_power_normal(const pm_sensors_arr *sensors)
{
    return pm_sensor_isNormal(&sensors->arr[SENSOR_VME_5V]);
}

bool get_input_power_failed(const pm_sensors_arr *sensors)
{
    return SENSOR_CRITICAL == pm_sensor_status(&sensors->arr[SENSOR_VME_5V]);
}

double pm_get_power_w(const Dev_powermon_priv *p)
{
    const pm_sensors_arr *sensors = &p->sensors;
    double mw = 0;
    mw += get_sensor_power_w(&sensors->arr[SENSOR_5VPC]);
    mw += get_sensor_power_w(&sensors->arr[SENSOR_VME_5V]);
    mw += get_sensor_power_w(&sensors->arr[SENSOR_VME_3V3]);
    return mw;
}

double pm_get_power_max_w(const Dev_powermon_priv *p)
{
    const pm_sensors_arr *sensors = &p->sensors;
    double mw = 0;
    mw += sensors->arr[SENSOR_5VPC].priv.powerMax;
    mw += sensors->arr[SENSOR_VME_5V].priv.powerMax;
    mw += sensors->arr[SENSOR_VME_3V3].priv.powerMax;
    return mw;
}

double pm_get_fpga_power_w(const Dev_powermon_priv *p)
{
    const pm_sensors_arr *sensors = &p->sensors;
    double mw = 0;
    mw += get_sensor_power_w(&sensors->arr[SENSOR_FPGA_CORE_1V0]);
    mw += get_sensor_power_w(&sensors->arr[SENSOR_FPGA_MGT_1V0]);
    mw += get_sensor_power_w(&sensors->arr[SENSOR_FPGA_MGT_1V2]);
    mw += get_sensor_power_w(&sensors->arr[SENSOR_FPGA_1V8]);
    return mw;
}

void bsp_update_system_powergood_pin(bool power_good)
{
    write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, power_good);
}

void switch_power(Dev_powermon_priv *p, bool state)
{
    // int pcb_ver = get_mcb_pcb_ver();
    if (state)
        log_put(LOG_NOTICE, "Switching ON");
    else
        log_put(LOG_NOTICE, "Switching OFF");
    for (int i=0; i<POWER_SWITCH_COUNT; i++)
        p->sw[i] = state;
//    p->sw[PSW_3V3] = 1;
//    p->sw[PSW_5V] = 1;
//    p->sw[PSW_2V5_CLK] = 1;

    write_power_switches(p->sw);
    if (state)
        osDelay(1); // allow 20 us for charge with pullups
}
