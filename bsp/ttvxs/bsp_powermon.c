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
