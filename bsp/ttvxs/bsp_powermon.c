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

// TTVXS board specific functions

#include "bsp_powermon.h"
#include "dev_pm_sensors_config.h"
#include "dev_powermon_types.h"
#include "dev_pm_sensors_types.h"
#include "dev_pm_sensors.h"
#include "bsp_pin_defs.h"

#include "gpio.h"
#include "stm32f7xx_hal_gpio.h"

int monIsOn(const pm_switches *sw, SensorIndex index)
{
    switch(index) {
    case SENSOR_VPC_3V3: return 1;
    case SENSOR_5VPC: return 1;
    case SENSOR_5V: return 1;
    case SENSOR_VXS_5V: return 1;
    case SENSOR_2V5: return sw->switch_2v5;
    case SENSOR_3V3: return sw->switch_3v3;
    case SENSOR_FPGA_CORE_1V0: return sw->switch_1v0_core;
    case SENSOR_FPGA_MGT_1V0: return sw->switch_1v0_mgt;
    case SENSOR_FPGA_MGT_1V2: return sw->switch_1v2_mgt; // TTVXS v1.0: sw->switch_2v5;
    case SENSOR_MCB_4V5: return 1;
    case SENSOR_FPGA_1V8: return sw->switch_2v5;
    case SENSOR_VADJ: return sw->switch_2v5;
    case SENSOR_FMC_5V: return sw->switch_5v_fmc;
    case SENSOR_FMC_12V: return sw->switch_5v_fmc;
    case SENSOR_CLOCK_2V5: return sw->switch_3v3;
    case SENSOR_MCB_3V3: return 1;
    }
    return 0;
}

void read_power_switches_state(pm_switches *sw_state)
{
    sw_state->switch_1v0_core = read_gpio_pin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin);
    sw_state->switch_1v0_mgt = read_gpio_pin(ON_1V0_MGT_GPIO_Port,  ON_1V0_MGT_Pin);
    sw_state->switch_1v2_mgt = read_gpio_pin(ON_1V2_MGT_GPIO_Port,  ON_1V2_MGT_Pin);
    sw_state->switch_2v5 = read_gpio_pin(ON_2V5_GPIO_Port,      ON_2V5_Pin);
    sw_state->switch_3v3 = read_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin);
    sw_state->switch_5v_fmc = read_gpio_pin(ON_FMC_5V_GPIO_Port,   ON_FMC_5V_Pin);
    sw_state->switch_5v = sw_state->switch_5v; // read_gpio_pin(ON_5V_VXS_GPIO_Port,   ON_5V_VXS_Pin);
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

static int read_pgood_2v5(void)
{
    return read_gpio_pin(PGOOD_2V5_GPIO_Port, PGOOD_2V5_Pin);
}

static int read_pgood_3v3(void)
{
    return read_gpio_pin(PGOOD_3V3_GPIO_Port, PGOOD_3V3_Pin);
}

static int read_pgood_3v3_fmc(void)
{
    return read_gpio_pin(PGOOD_FMC_3P3VAUX_GPIO_Port, PGOOD_FMC_3P3VAUX_Pin);
}

void pm_read_pgood(pm_pgoods *pgood)
{
    pgood->pgood_1v0_core = read_pgood_1v0_core();
    pgood->pgood_1v0_mgt  = read_pgood_1v0_mgt();
    pgood->pgood_1v2_mgt  = read_pgood_1v2_mgt();
    pgood->pgood_2v5      = read_pgood_2v5();
    pgood->pgood_3v3      = read_pgood_3v3();
    pgood->pgood_3v3_fmc  = read_pgood_3v3_fmc();
}

bool get_all_pgood(const pm_pgoods *pgood)
{
    return pgood->pgood_1v0_core
           && pgood->pgood_1v0_mgt
           && pgood->pgood_1v2_mgt
           && pgood->pgood_2v5
           && pgood->pgood_3v3
           && pgood->pgood_3v3_fmc;
}

bool get_input_power_valid(const pm_sensors_arr sensors)
{
    return pm_sensor_isValid(&sensors[SENSOR_VXS_5V]);
}

bool get_input_power_normal(const pm_sensors_arr sensors)
{
    return pm_sensor_isNormal(&sensors[SENSOR_VXS_5V]);
}

bool get_input_power_failed(const pm_sensors_arr sensors)
{
    return SENSOR_CRITICAL == pm_sensor_status(&sensors[SENSOR_VXS_5V]);
}
