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

// TDC72VHL v4 board specific functions

#include "bsp_powermon.h"
#include "dev_pm_sensors_config.h"
#include "dev_powermon_types.h"
#include "dev_pm_sensors_types.h"
#include "dev_pm_sensors.h"
#include "bsp_pin_defs.h"

#include "cmsis_os.h"
#include "gpio.h"
#include "stm32f7xx_hal_gpio.h"

int monIsOn(const pm_switches sw, SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5: return sw[PSW_1V5];
    case SENSOR_5V: return sw[PSW_5V];
    case SENSOR_5VPC: return 1;
    case SENSOR_VME_5V: return 1;
    case SENSOR_3V3: return sw[PSW_3V3];
    case SENSOR_VME_3V3: return 1;
    case SENSOR_VMCU: return 1;
    case SENSOR_FPGA_CORE_1V0: return sw[PSW_1V0];
    case SENSOR_FPGA_MGT_1V0: return sw[PSW_1V0];
    case SENSOR_FPGA_MGT_1V2: return sw[PSW_1V0];
    case SENSOR_FPGA_1V8: return sw[PSW_3V3];
    case SENSOR_TDC_A: return sw[PSW_3V3];
    case SENSOR_TDC_B: return sw[PSW_3V3];
    case SENSOR_TDC_C: return sw[PSW_3V3];
    case SENSOR_TDC_D: return sw[PSW_3V3];
    case SENSOR_CLOCK_2V5: return sw[PSW_3V3];
    }
    return 0;
}

bool readLiveInsertPin(void)
{
    bool state = read_gpio_pin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (false == state);
}

void init_power_switches(pm_switches sw)
{
    for (int i=0; i<POWER_SWITCH_COUNT; i++)
        sw[i] = true;
}

void read_power_switches_state(pm_switches sw_state)
{
    sw_state[PSW_1V0]   = read_gpio_pin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin);
    sw_state[PSW_1V5]   = read_gpio_pin(ON_1_5V_GPIO_Port, ON_1_5V_Pin);
    sw_state[PSW_3V3]   = read_gpio_pin(ON_3_3V_GPIO_Port, ON_3_3V_Pin);
    sw_state[PSW_5V]    = read_gpio_pin(ON_5V_GPIO_Port,   ON_5V_Pin);
    sw_state[PSW_TDC_A] = read_gpio_pin(ON_TDC_A_GPIO_Port, ON_TDC_A_Pin);
    sw_state[PSW_TDC_B] = read_gpio_pin(ON_TDC_B_GPIO_Port, ON_TDC_B_Pin);
    sw_state[PSW_TDC_C] = read_gpio_pin(ON_TDC_C_GPIO_Port, ON_TDC_C_Pin);
    sw_state[PSW_TDC_D] = read_gpio_pin(ON_TDC_D_GPIO_Port, ON_TDC_D_Pin);
}

void write_power_switches(pm_switches sw)
{
    write_gpio_pin(ON_5V_GPIO_Port,        ON_5V_Pin,        sw[PSW_5V]);
    write_gpio_pin(ON_1_5V_GPIO_Port,      ON_1_5V_Pin,      sw[PSW_1V5]);
    write_gpio_pin(ON_3_3V_GPIO_Port,      ON_3_3V_Pin,      sw[PSW_3V3]);
    write_gpio_pin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin, sw[PSW_1V0]);
    write_gpio_pin(ON_TDC_A_GPIO_Port,     ON_TDC_A_Pin,     sw[PSW_TDC_A]);
    write_gpio_pin(ON_TDC_B_GPIO_Port,     ON_TDC_B_Pin,     sw[PSW_TDC_B]);
    write_gpio_pin(ON_TDC_C_GPIO_Port,     ON_TDC_C_Pin,     sw[PSW_TDC_C]);
    write_gpio_pin(ON_TDC_D_GPIO_Port,     ON_TDC_D_Pin,     sw[PSW_TDC_D]);
}

void init_pgood(pm_pgoods pgood)
{
    for (int i=0; i<POWER_GOOD_COUNT; i++)
        pgood[i] = false;
}

static bool readPowerGoodFpga(void)
{
    return read_gpio_pin(FPGA_CORE_PGOOD_GPIO_Port, FPGA_CORE_PGOOD_Pin);
}

static bool readPowerGood1v5(void)
{
    return read_gpio_pin(LTM_PGOOD_GPIO_Port, LTM_PGOOD_Pin);
}

void pm_read_pgood(pm_pgoods pgood)
{
    pgood[PGOOD_1V0] = readPowerGoodFpga();
    pgood[PGOOD_1V5]= readPowerGood1v5();
}

bool get_all_pgood(const pm_pgoods pgood)
{
    for (int i=0; i<POWER_GOOD_COUNT; i++)
        if (!pgood[i])
            return false;
    return true;
}

bool get_input_power_valid(const pm_sensors_arr sensors)
{
    return pm_sensor_isValid(&sensors[SENSOR_VME_5V]);
}

bool get_input_power_normal(const pm_sensors_arr sensors)
{
    return pm_sensor_isNormal(&sensors[SENSOR_VME_5V]);
}

bool get_input_power_failed(const pm_sensors_arr sensors)
{
    return SENSOR_CRITICAL == pm_sensor_status(&sensors[SENSOR_VME_5V]);
}

double pm_get_power_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += get_sensor_power_w(&pm->sensors[SENSOR_5VPC]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_VME_5V]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_VME_3V3]);
    return mw;
}

double pm_get_power_max_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += pm->sensors[SENSOR_5VPC].powerMax;
    mw += pm->sensors[SENSOR_VME_5V].powerMax;
    mw += pm->sensors[SENSOR_VME_3V3].powerMax;
    return mw;
}

void bsp_update_system_powergood_pin(bool power_good)
{
    // write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, power_good);
}

void switch_power(Dev_powermon *pm, bool state)
{
    // turn off only when failed
    //    bool state = state_primary;

    bool state_primary = (pm->pmState != PM_STATE_PWRFAIL) && (pm->pmState != PM_STATE_OFF) && (pm->pmState != PM_STATE_OVERHEAT);

    // primary switches (required for monitors)
    pm->sw[PSW_5V]  = state_primary; // VME 5V and 3.3V
    pm->sw[PSW_3V3] = state_primary;
    write_power_switches(pm->sw);

    // secondary switches
    bool turnon_1v5 = !pm->sw[PSW_1V5] && state;
    pm->sw[PSW_1V5] = state;
    write_power_switches(pm->sw);
    if (turnon_1v5)
        osDelay(10);
    pm->sw[PSW_1V0] = state;
    pm->sw[PSW_TDC_A] = true; // state;
    pm->sw[PSW_TDC_B] = true; // state;
    pm->sw[PSW_TDC_C] = true; // state;
    pm->sw[PSW_TDC_D] = true; // state;

    write_power_switches(pm->sw);
    if (state)
        osDelay(1); // allow 20 us for charge with pullups
}
