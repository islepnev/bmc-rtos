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

// TDC72VHL v3 board specific functions

#include "bsp_powermon.h"

#include "bsp_pin_defs.h"
#include "cmsis_os.h"
#include "bsp_sensors_config.h"
#include "gpio.h"
#include "powermon/dev_pm_sensors.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon_types.h"
#include "stm32f7xx_hal_gpio.h"

int monIsOn(const pm_switches sw, SensorIndex index)
{
    (void)sw;
    (void)index;
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
    sw_state[PSW_1V0]   = read_gpio_pin(ON_1V0_1V2_GPIO_Port, ON_1V0_1V2_Pin);
    sw_state[PSW_1V5]   = read_gpio_pin(ON_1V5_GPIO_Port, ON_1V5_Pin);
    sw_state[PSW_3V3]   = read_gpio_pin(ON_3V3_GPIO_Port, ON_3V3_Pin);
    sw_state[PSW_5V]    = read_gpio_pin(ON_5V_GPIO_Port,   ON_5V_Pin);
}

void write_power_switches(pm_switches sw)
{
    write_gpio_pin(ON_5V_GPIO_Port,        ON_5V_Pin,        sw[PSW_5V]);
    write_gpio_pin(ON_1V5_GPIO_Port,      ON_1V5_Pin,      sw[PSW_1V5]);
    write_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin,      sw[PSW_3V3]);
    write_gpio_pin(ON_1V0_1V2_GPIO_Port, ON_1V0_1V2_Pin, sw[PSW_1V0]);
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

bool get_input_power_valid(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return true;
}

bool get_input_power_normal(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return true;
}

bool get_input_power_failed(const pm_sensors_arr *sensors)
{
    (void)sensors;
    return false;
}

double pm_get_power_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

double pm_get_power_max_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

double pm_get_fpga_power_w(const Dev_powermon_priv *p)
{
    (void)p;
    return 0;
}

void bsp_update_system_powergood_pin(bool power_good)
{
    // write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, power_good);
}

void switch_power(Dev_powermon_priv *p, bool state)
{
    // turn off only when failed
    //    bool state = state_primary;

    bool state_primary = (p->pmState != PM_STATE_PWRFAIL) &&
                         (p->pmState != PM_STATE_OFF) &&
                         (p->pmState != PM_STATE_OVERHEAT);

    // primary switches (required for monitors)
    p->sw[PSW_5V]  = state_primary; // VME 5V and 3.3V
    p->sw[PSW_3V3] = state_primary;
    write_power_switches(p->sw);

    // secondary switches
    bool turnon_1v5 = !p->sw[PSW_1V5] && state;
    p->sw[PSW_1V5] = state;
    write_power_switches(p->sw);
    if (turnon_1v5)
        osDelay(10);
    p->sw[PSW_1V0] = state;

    write_power_switches(p->sw);
    if (state)
        osDelay(1); // allow 20 us for charge with pullups
}
