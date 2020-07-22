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

#include "dev_pm_sensors_config.h"

int monIsOptional(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0;
    case SENSOR_5VPC:          return 1; // FIXME
    case SENSOR_5V:            return 0;
    case SENSOR_VME_5V:        return 0;
    case SENSOR_3V3:           return 0;
    case SENSOR_VME_3V3:       return 0;
    case SENSOR_FPGA_CORE_1V0: return 0;
    case SENSOR_FPGA_MGT_1V0:  return 0;
    case SENSOR_FPGA_MGT_1V2:  return 0;
    case SENSOR_VMCU:          return 1; // FIXME
    case SENSOR_FPGA_1V8:      return 0;
    case SENSOR_TDC_A:         return 1;
    case SENSOR_TDC_B:         return 1;
    case SENSOR_TDC_C:         return 1;
    case SENSOR_TDC_D:         return 1;
    case SENSOR_CLOCK_2V5:     return 0;
    }
    return 0;
}

double monShuntVal(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.002;
    case SENSOR_5VPC:          return 0.002;
    case SENSOR_5V:            return 0;
    case SENSOR_VME_5V:        return 0.010;
    case SENSOR_3V3:           return 0;
    case SENSOR_VME_3V3:       return 0.004;
    case SENSOR_FPGA_CORE_1V0: return 0.002;
    case SENSOR_FPGA_MGT_1V0:  return 0.002;
    case SENSOR_FPGA_MGT_1V2:  return 0.002;
    case SENSOR_VMCU:          return 0.002;
    case SENSOR_FPGA_1V8:      return 0.002;
    case SENSOR_TDC_A:         return 0.002;
    case SENSOR_TDC_B:         return 0.002;
    case SENSOR_TDC_C:         return 0.002;
    case SENSOR_TDC_D:         return 0.002;
    case SENSOR_CLOCK_2V5:     return 0.002;
    }
    return 0;
}

double monVoltageMarginWarn(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.03;
    case SENSOR_5VPC:          return 0.1;
    case SENSOR_5V:            return 0.1;
    case SENSOR_VME_5V:        return 0.1;
    case SENSOR_3V3:           return 0.1;
    case SENSOR_VME_3V3:       return 0.1;
    case SENSOR_FPGA_CORE_1V0: return 0.03;
    case SENSOR_FPGA_MGT_1V0:  return 0.03;
    case SENSOR_FPGA_MGT_1V2:  return 0.03;
    case SENSOR_VMCU:          return 0.1;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_TDC_A:         return 0.1;
    case SENSOR_TDC_B:         return 0.1;
    case SENSOR_TDC_C:         return 0.1;
    case SENSOR_TDC_D:         return 0.1;
    case SENSOR_CLOCK_2V5:     return 0.03;
    }
    return 0;
}

double monVoltageMarginCrit(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.1;
    case SENSOR_5VPC:          return 0.15;
    case SENSOR_5V:            return 0.15;
    case SENSOR_VME_5V:        return 0.15;
    case SENSOR_3V3:           return 0.15;
    case SENSOR_VME_3V3:       return 0.15;
    case SENSOR_FPGA_CORE_1V0: return 0.08; // set to 8 %
    case SENSOR_FPGA_MGT_1V0:  return 0.05;
    case SENSOR_FPGA_MGT_1V2:  return 0.05;
    case SENSOR_VMCU:          return 0.15;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_TDC_A:         return 0.15;
    case SENSOR_TDC_B:         return 0.15;
    case SENSOR_TDC_C:         return 0.15;
    case SENSOR_TDC_D:         return 0.15;
    case SENSOR_CLOCK_2V5:     return 0.05;
    }
    return 0;
}

double monVoltageNom(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 1.5;
    case SENSOR_5VPC:          return 5.0;
    case SENSOR_5V:            return 5.0;
    case SENSOR_VME_5V:        return 5.0;
    case SENSOR_3V3:           return 3.3;
    case SENSOR_VME_3V3:       return 3.3;
    case SENSOR_FPGA_CORE_1V0: return 1.0;
    case SENSOR_FPGA_MGT_1V0:  return 1.0;
    case SENSOR_FPGA_MGT_1V2:  return 1.2;
    case SENSOR_VMCU:          return 4.6;
    case SENSOR_FPGA_1V8:      return 1.8;
    case SENSOR_TDC_A:         return 2.5;
    case SENSOR_TDC_B:         return 2.5;
    case SENSOR_TDC_C:         return 2.5;
    case SENSOR_TDC_D:         return 2.5;
    case SENSOR_CLOCK_2V5:     return 2.5;
    }
    return 0;
}

int sensorBusAddress(SensorIndex index)
{
    switch (index) {
    case SENSOR_1V5:           return 0x40;
    case SENSOR_5VPC:          return 0x41;
    case SENSOR_5V:            return 0x42;
    case SENSOR_VME_5V:        return 0x43;
    case SENSOR_3V3:           return 0x44;
    case SENSOR_VME_3V3:       return 0x45;
    case SENSOR_FPGA_CORE_1V0: return 0x46;
    case SENSOR_FPGA_MGT_1V0:  return 0x47;
    case SENSOR_FPGA_MGT_1V2:  return 0x48;
    case SENSOR_VMCU:          return 0x49;
    case SENSOR_FPGA_1V8:      return 0x4A;
    case SENSOR_TDC_A:         return 0x4B;
    case SENSOR_TDC_B:         return 0x4C; // maybe 0x4D
    case SENSOR_TDC_C:         return 0x4D; // maybe 0x4C
    case SENSOR_CLOCK_2V5:     return 0x4E;
    case SENSOR_TDC_D:         return 0x4F;
    }
    return 0;
}

const char *monLabel(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return   "int 1.5V"; // U1, ? mOhm
    case SENSOR_5VPC:          return       "5VPC";
    case SENSOR_5V:            return         "5V"; // U3
    case SENSOR_VME_5V:        return     "VME 5V"; // U5, 10 mOhm
    case SENSOR_3V3:           return       "3.3V"; // U7
    case SENSOR_VME_3V3:       return   "VME 3.3V"; // U9, 4 mOhm
    case SENSOR_FPGA_CORE_1V0: return  "Core 1.0V"; // U11, 2 mOhm
    case SENSOR_FPGA_MGT_1V0:  return   "MGT 1.0V"; // U13, 2 mOhm
    case SENSOR_FPGA_MGT_1V2:  return   "MGT 1.2V"; // U2, 2 mOhm
    case SENSOR_VMCU:          return   "MCU 4.5V";
    case SENSOR_FPGA_1V8:      return   "FPGA 1.8"; // U, 2 mOhm
    case SENSOR_TDC_A:         return  "TDC-A 2.5"; // U, 2 mOhm
    case SENSOR_TDC_B:         return  "TDC-B 2.5"; // U, 2 mOhm
    case SENSOR_TDC_C:         return  "TDC-C 2.5"; // U, 2 mOhm
    case SENSOR_TDC_D:         return  "TDC-D 2.5"; // U, 2 mOhm
    case SENSOR_CLOCK_2V5:     return  "Clock 2.5"; // U, 2 mOhm
    }
    return "???";
}
