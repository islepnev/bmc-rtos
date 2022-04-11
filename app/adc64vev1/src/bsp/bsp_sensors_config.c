/*
**    Copyright 2019-2021 Ilja Slepnev
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

// ADC64VE board specific functions

#include "bsp_sensors_config.h"

#include <math.h>

#include "bus/bus_types.h"
#include "powermon/dev_pm_sensors_types.h"

const SensorIndex input_power_sensor = SENSOR_UNUSED;

static bool monIsOptional(SensorIndex index)
{
    return 0;
}

static double monShuntVal(SensorIndex index)
{
    return 0;
}

static double monVoltageMarginWarn(SensorIndex index)
{
    return 0;
}

static double monVoltageMarginCrit(SensorIndex index)
{
    return 0;
}

static double monVoltageNom(SensorIndex index)
{
    return 0;
}

static int sensorBusNumber(SensorIndex index)
{
    (void)index;
    return 0;
}

static int sensorBusAddress(SensorIndex index)
{
    return 0;
}

static const char *monLabel(SensorIndex index)
{
    return "???";
}

void bsp_pm_sensors_arr_init(pm_sensors_arr *arr)
{
    arr->count = POWERMON_SENSORS;
}
