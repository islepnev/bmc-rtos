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

#include "bsp_sensors_config.h"

#include <math.h>

#include "bus/bus_types.h"
#include "powermon/dev_pm_sensors_types.h"

const SensorIndex input_power_sensor = SENSOR_VME_5V;

static bool monIsOptional(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return 1;
    case SENSOR_TDCA_2V5:      return 1;
    case SENSOR_CLOCK_2V5:     return 0;
    case SENSOR_CLOCK_3V3:     return 0;
    case SENSOR_FPGA_1V8:      return 0;
    case SENSOR_FPGA_CORE_1V0: return 0;
    case SENSOR_FPGA_MGT_1V0:  return 0;
    case SENSOR_FPGA_MGT_1V2:  return 0;
    case SENSOR_MCB_3V3:       return 1;
    case SENSOR_MCB_4V5:       return 1;
    case SENSOR_INT_3V3:       return 0;
    case SENSOR_VME_5V:        return 0;
    }
    return 0;
}

static double monShuntVal(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return 0.002;
    case SENSOR_TDCA_2V5:      return 0.002;
    case SENSOR_CLOCK_2V5:     return 0.002;
    case SENSOR_CLOCK_3V3:     return 0.002;
    case SENSOR_FPGA_1V8:      return 0.002;
    case SENSOR_FPGA_CORE_1V0: return 0.002;
    case SENSOR_FPGA_MGT_1V0:  return 0.002;
    case SENSOR_FPGA_MGT_1V2:  return 0.002;
    case SENSOR_MCB_3V3:       return 0.002;
    case SENSOR_MCB_4V5:       return 0.002;
    case SENSOR_INT_3V3:       return 0.002;
    case SENSOR_VME_5V:        return 0.004;
    }
    return 0;
}

static double monVoltageMarginWarn(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return 0.10;
    case SENSOR_TDCA_2V5:      return 0.10;
    case SENSOR_CLOCK_2V5:     return 0.03;
    case SENSOR_CLOCK_3V3:     return 0.03;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_FPGA_CORE_1V0: return 0.03;
    case SENSOR_FPGA_MGT_1V0:  return 0.03;
    case SENSOR_FPGA_MGT_1V2:  return 0.03;
    case SENSOR_MCB_3V3:       return 0.1;
    case SENSOR_MCB_4V5:       return 0.1;
    case SENSOR_INT_3V3:       return 0.1;
    case SENSOR_VME_5V:        return 0.1;
    }
    return 0;
}

static double monVoltageMarginCrit(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return 0.15;
    case SENSOR_TDCA_2V5:      return 0.15;
    case SENSOR_CLOCK_2V5:     return 0.05;
    case SENSOR_CLOCK_3V3:     return 0.05;
    case SENSOR_FPGA_1V8:      return 0.10;
    case SENSOR_FPGA_CORE_1V0: return 0.05;
    case SENSOR_FPGA_MGT_1V0:  return 0.05;
    case SENSOR_FPGA_MGT_1V2:  return 0.05;
    case SENSOR_MCB_3V3:       return 0.15;
    case SENSOR_MCB_4V5:       return 0.15;
    case SENSOR_INT_3V3:       return 0.15;
    case SENSOR_VME_5V:        return 0.15;
    }
    return 0;
}

static double monVoltageNom(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return 2.5;
    case SENSOR_TDCA_2V5:      return 2.5;
    case SENSOR_CLOCK_2V5:     return 2.5;
    case SENSOR_CLOCK_3V3:     return 3.3;
    case SENSOR_FPGA_1V8:      return 1.8;
    case SENSOR_FPGA_CORE_1V0: return 1.0;
    case SENSOR_FPGA_MGT_1V0:  return 1.0;
    case SENSOR_FPGA_MGT_1V2:  return 1.2;
    case SENSOR_MCB_3V3:       return 3.3;
    case SENSOR_MCB_4V5:       return 4.6;
    case SENSOR_INT_3V3:       return 3.3;
    case SENSOR_VME_5V:        return 5.0;
    }
    return 0;
}

static int sensorBusNumber(SensorIndex index)
{
    (void)index;
    return 2;
}

static int sensorBusAddress(SensorIndex index)
{
    switch (index) {
    case SENSOR_TDCB_2V5:      return 0x42;
    case SENSOR_TDCA_2V5:      return 0x41;
    case SENSOR_CLOCK_2V5:     return 0x4E;
    case SENSOR_CLOCK_3V3:     return 0x4C;
    case SENSOR_FPGA_1V8:      return 0x4A;
    case SENSOR_FPGA_CORE_1V0: return 0x46;
    case SENSOR_FPGA_MGT_1V0:  return 0x47;
    case SENSOR_FPGA_MGT_1V2:  return 0x48;
    case SENSOR_MCB_3V3:       return 0x4F;
    case SENSOR_MCB_4V5:       return 0x49;
    case SENSOR_INT_3V3:       return 0x45;
    case SENSOR_VME_5V:        return 0x43;
    }
    return 0;
}

static const char *monLabel(SensorIndex index)
{
    switch(index) {
    case SENSOR_TDCB_2V5:      return "TDC-A 2.5";
    case SENSOR_TDCA_2V5:      return "TDC-B 2.5";
    case SENSOR_CLOCK_2V5:     return "CLK 2.5";
    case SENSOR_CLOCK_3V3:     return "CLK 3.3";
    case SENSOR_FPGA_1V8:      return "FPGA 1.8";
    case SENSOR_FPGA_CORE_1V0: return "FPGA 1.0";
    case SENSOR_FPGA_MGT_1V0:  return "MGT 1.0";
    case SENSOR_FPGA_MGT_1V2:  return "MGT 1.2";
    case SENSOR_MCB_3V3:       return "MCB 3.3";
    case SENSOR_MCB_4V5:       return "MCB 4.5";
    case SENSOR_INT_3V3:       return "INT 3.3";
    case SENSOR_VME_5V:        return "VME  +5";
    }
    return "???";
}

void bsp_pm_sensors_arr_init(pm_sensors_arr *arr)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *d = &arr->arr[i];
        d->dev.device_status = DEVICE_UNKNOWN;
        d->dev.bus.type = BUS_IIC;
        d->dev.bus.bus_number = sensorBusNumber(i);
        d->dev.bus.address = sensorBusAddress(i);
        pm_sensor_priv *sensor = &d->priv;
        sensor->index = i;
        sensor->isOptional = monIsOptional(i);
        sensor->hasShunt = fabs(monShuntVal(i)) > 1e-6;
        sensor->shuntVal = monShuntVal(i);
        sensor->busNomVoltage = monVoltageNom(i);
        sensor->label = monLabel(i);
        sensor->voltageMarginWarn = monVoltageMarginWarn(i);
        sensor->voltageMarginCrit = monVoltageMarginCrit(i);
        // struct_pm_sensor_clear(d);
    }
    arr->count = POWERMON_SENSORS;
}
