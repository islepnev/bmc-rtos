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

#include "dev_pm_sensors_config.h"

const SensorIndex input_power_sensor = SENSOR_VME_5V;

bool monIsOptional(SensorIndex index)
{
    switch(index) {
    case SENSOR_5VPC:          return 1;
    case SENSOR_VME_3V3:       return 1; // VME drivers pre-charge only
    case SENSOR_5V:            return 0;
    case SENSOR_VME_5V:        return 0;
    case SENSOR_1V5:           return 0;
    case SENSOR_3V3:           return 0;
    case SENSOR_FPGA_CORE_1V0: return 0;
    case SENSOR_FPGA_MGT_1V0:  return 0;
    case SENSOR_FPGA_MGT_1V2:  return 0;
    case SENSOR_MCB_4V5:       return 1;
    case SENSOR_FPGA_1V8:      return 0;
    case SENSOR_CLK_2V5:       return 0;
    case SENSOR_CLK_3V3:       return 1;
    case SENSOR_MCB_3V3:       return 1;
    }
    return 0;
}

double monShuntVal(SensorIndex index)
{
    switch(index) {
    case SENSOR_5VPC:          return 0.002;
    case SENSOR_VME_3V3:       return 0.004;
    case SENSOR_5V:            return 0; // no shunt
    case SENSOR_VME_5V:        return 0.004;
    case SENSOR_1V5:           return 0.002;
    case SENSOR_3V3:           return 0; // no shunt
    case SENSOR_FPGA_CORE_1V0: return 0.002;
    case SENSOR_FPGA_MGT_1V0:  return 0.002;
    case SENSOR_FPGA_MGT_1V2:  return 0.002;
    case SENSOR_MCB_4V5:       return 0.002;
    case SENSOR_FPGA_1V8:      return 0.002;
    case SENSOR_CLK_2V5:       return 0.002;
    case SENSOR_CLK_3V3:       return 0.01;
    case SENSOR_MCB_3V3:       return 0.002;
    }
    return 0;
}

double monVoltageMarginWarn(SensorIndex index)
{
    switch(index) {
    case SENSOR_5VPC:          return 0.1;
    case SENSOR_VME_3V3:       return 0.1;
    case SENSOR_5V:            return 0.1;
    case SENSOR_VME_5V:        return 0.1;
    case SENSOR_1V5:           return 0.1;
    case SENSOR_3V3:           return 0.03;
    case SENSOR_FPGA_CORE_1V0: return 0.03;
    case SENSOR_FPGA_MGT_1V0:  return 0.03;
    case SENSOR_FPGA_MGT_1V2:  return 0.03;
    case SENSOR_MCB_4V5:       return 0.1;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_CLK_2V5:       return 0.03;
    case SENSOR_CLK_3V3:       return 0.1;
    case SENSOR_MCB_3V3:       return 0.1;
    }
    return 0;
}

double monVoltageMarginCrit(SensorIndex index)
{
    switch(index) {
    case SENSOR_5VPC:          return 0.15;
    case SENSOR_VME_3V3:       return 0.15;
    case SENSOR_5V:            return 0.15;
    case SENSOR_VME_5V:        return 0.15;
    case SENSOR_1V5:           return 0.15;
    case SENSOR_3V3:           return 0.15;
    case SENSOR_FPGA_CORE_1V0: return 0.08; // set to 8 %
    case SENSOR_FPGA_MGT_1V0:  return 0.05;
    case SENSOR_FPGA_MGT_1V2:  return 0.05;
    case SENSOR_MCB_4V5:       return 0.15;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_CLK_2V5:       return 0.05;
    case SENSOR_CLK_3V3:       return 0.15;
    case SENSOR_MCB_3V3:       return 0.15;
    }
    return 0;
}

double monVoltageNom(SensorIndex index)
{
    switch(index) {
    case SENSOR_5VPC:          return 5.0;
    case SENSOR_VME_3V3:       return 3.3;
    case SENSOR_5V:            return 5.0;
    case SENSOR_VME_5V:        return 5.0;
    case SENSOR_1V5:           return 1.5;
    case SENSOR_3V3:           return 3.3;
    case SENSOR_FPGA_CORE_1V0: return 1.0;
    case SENSOR_FPGA_MGT_1V0:  return 1.0;
    case SENSOR_FPGA_MGT_1V2:  return 1.2;
    case SENSOR_MCB_4V5:       return 4.6;
    case SENSOR_FPGA_1V8:      return 1.8;
    case SENSOR_CLK_2V5:       return 2.5;
    case SENSOR_CLK_3V3:       return 3.3;
    case SENSOR_MCB_3V3:       return 3.3;
    }
    return 0;
}

int sensorBusAddress(SensorIndex index)
{
    switch (index) {
    case SENSOR_3V3:           return 0x40;
    case SENSOR_5VPC:          return 0x41;
    case SENSOR_5V:            return 0x42;
    case SENSOR_VME_5V:        return 0x43;
    case SENSOR_1V5:           return 0x44;
    case SENSOR_VME_3V3:       return 0x45;
    case SENSOR_FPGA_CORE_1V0: return 0x46;
    case SENSOR_FPGA_MGT_1V0:  return 0x47;
    case SENSOR_FPGA_MGT_1V2:  return 0x48;
    case SENSOR_MCB_4V5:       return 0x49;
    case SENSOR_FPGA_1V8:      return 0x4A;
    case SENSOR_CLK_3V3:       return 0x4C;
    case SENSOR_CLK_2V5:       return 0x4E;
    case SENSOR_MCB_3V3:       return 0x4F;
    }
    return 0;
}

const char *monLabel(SensorIndex index)
{
    switch(index) {
    case SENSOR_VME_3V3:       return "VME 3.3V";
    case SENSOR_5VPC:          return "VPC 5V";
    case SENSOR_5V:            return "5V";
    case SENSOR_VME_5V:        return "VME 5V";
    case SENSOR_1V5:           return "1.5V";
    case SENSOR_3V3:           return "3.3V";
    case SENSOR_FPGA_CORE_1V0: return "Core 1.0V";
    case SENSOR_FPGA_MGT_1V0:  return "MGT 1.0V";
    case SENSOR_FPGA_MGT_1V2:  return "MGT 1.2V";
    case SENSOR_MCB_4V5:       return "MCB 4.5V";
    case SENSOR_FPGA_1V8:      return "FPGA 1.8V";
    case SENSOR_CLK_2V5:       return "CLK 2.5V";
    case SENSOR_CLK_3V3:       return "CLK 3.3V";
    case SENSOR_MCB_3V3:       return "MCB 3.3V";
    }
    return "???";
}
