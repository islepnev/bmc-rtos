//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dev_pm_sensors.h"
#include "ina226_i2c_hal.h"

#include "ansi_escape_codes.h"
#include "display.h"
#include "ftoa.h"
#include "dev_mcu.h"

//const float SENSOR_VOLTAGE_MARGIN_WARN = 0.05;
const float SENSOR_VOLTAGE_MARGIN_CRIT = 0.1;

float monShuntVal(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.002;
    case SENSOR_5V:            return 0;
    case SENSOR_VME_5V:        return 0.010;
    case SENSOR_3V3:           return 0;
    case SENSOR_VME_3V3:       return 0.004;
    case SENSOR_FPGA_CORE_1V0: return 0.002;
    case SENSOR_FPGA_MGT_1V0:  return 0.002;
    case SENSOR_FPGA_MGT_1V2:  return 0.002;
    case SENSOR_FPGA_1V8:      return 0.002;
    case SENSOR_TDC_A:         return 0.002;
    case SENSOR_TDC_B:         return 0.002;
    case SENSOR_TDC_C:         return 0.002;
    case SENSOR_CLOCK_2V5:     return 0.002;
    }
    return 0;
}

float monVoltageMarginWarn(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.03;
    case SENSOR_5V:            return 0.1;
    case SENSOR_VME_5V:        return 0.1;
    case SENSOR_3V3:           return 0.1;
    case SENSOR_VME_3V3:       return 0.1;
    case SENSOR_FPGA_CORE_1V0: return 0.03;
    case SENSOR_FPGA_MGT_1V0:  return 0.03;
    case SENSOR_FPGA_MGT_1V2:  return 0.03;
    case SENSOR_FPGA_1V8:      return 0.03;
    case SENSOR_TDC_A:         return 0.1;
    case SENSOR_TDC_B:         return 0.1;
    case SENSOR_TDC_C:         return 0.1;
    case SENSOR_CLOCK_2V5:     return 0.03;
    }
    return 0;
}

float monVoltageMarginCrit(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 0.1;
    case SENSOR_5V:            return 0.15;
    case SENSOR_VME_5V:        return 0.15;
    case SENSOR_3V3:           return 0.15;
    case SENSOR_VME_3V3:       return 0.15;
    case SENSOR_FPGA_CORE_1V0: return 0.05;
    case SENSOR_FPGA_MGT_1V0:  return 0.05;
    case SENSOR_FPGA_MGT_1V2:  return 0.05;
    case SENSOR_FPGA_1V8:      return 0.05;
    case SENSOR_TDC_A:         return 0.15;
    case SENSOR_TDC_B:         return 0.15;
    case SENSOR_TDC_C:         return 0.15;
    case SENSOR_CLOCK_2V5:     return 0.05;
    }
    return 0;
}

float monVoltageNom(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return 1.5;
    case SENSOR_5V:            return 5.0;
    case SENSOR_VME_5V:        return 5.0;
    case SENSOR_3V3:           return 3.3;
    case SENSOR_VME_3V3:       return 3.3;
    case SENSOR_FPGA_CORE_1V0: return 1.0;
    case SENSOR_FPGA_MGT_1V0:  return 1.0;
    case SENSOR_FPGA_MGT_1V2:  return 1.2;
    case SENSOR_FPGA_1V8:      return 1.8;
    case SENSOR_TDC_A:         return 2.6;
    case SENSOR_TDC_B:         return 2.6;
    case SENSOR_TDC_C:         return 2.6;
    case SENSOR_CLOCK_2V5:     return 2.5;
    }
    return 0;
}

int sensorBusAddress(SensorIndex index)
{
    switch (index) {
    case SENSOR_1V5:           return 0x40;
    case SENSOR_5V:            return 0x42;
    case SENSOR_VME_5V:        return 0x43;
    case SENSOR_3V3:           return 0x44;
    case SENSOR_VME_3V3:       return 0x45;
    case SENSOR_FPGA_CORE_1V0: return 0x46;
    case SENSOR_FPGA_MGT_1V0:  return 0x47;
    case SENSOR_FPGA_MGT_1V2:  return 0x48;
    case SENSOR_FPGA_1V8:      return 0x4A;
    case SENSOR_TDC_A:         return 0x4B;
    case SENSOR_TDC_B:         return 0x4C;
    case SENSOR_TDC_C:         return 0x4D;
    case SENSOR_CLOCK_2V5:     return 0x4E;
    }
    return 0;
}

const char *monLabel(SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5:           return "  int 1.5V"; // U1, ? mOhm
    case SENSOR_5V:            return "        5V"; // U3
    case SENSOR_VME_5V:        return "    VME 5V"; // U5, 10 mOhm
    case SENSOR_3V3:           return "      3.3V"; // U7
    case SENSOR_VME_3V3:       return "  VME 3.3V"; // U9, 4 mOhm
    case SENSOR_FPGA_CORE_1V0: return "  FPGA 1.0"; // U11, 2 mOhm
    case SENSOR_FPGA_MGT_1V0:  return "   MGT 1.0"; // U13, 2 mOhm
    case SENSOR_FPGA_MGT_1V2:  return "   MGT 1.2"; // U2, 2 mOhm
    case SENSOR_FPGA_1V8:      return "  FPGA 1.8"; // U, 2 mOhm
    case SENSOR_TDC_A:         return " TDC-A 2.5"; // U, 2 mOhm
    case SENSOR_TDC_B:         return " TDC-B 2.5"; // U, 2 mOhm
    case SENSOR_TDC_C:         return " TDC-C 2.5"; // U, 2 mOhm
    case SENSOR_CLOCK_2V5:     return " Clock 2.5"; // U, 2 mOhm
    }
    return "???";
}

void struct_pm_sensor_clear_measurements(pm_sensor *d)
{
    d->busVoltage = 0;
    d->shuntVoltage = 0;
    d->current = 0;
}

void struct_pm_sensor_init(pm_sensor *d, SensorIndex index)
{
    d->index = index;
    d->deviceStatus = DEVICE_UNKNOWN;
    d->sensorStatus = SENSOR_NORMAL;
    d->lastStatusUpdatedTick = 0;
    d->busAddress = sensorBusAddress(index);
    d->hasShunt = monShuntVal(index) > 1e-6f;
    d->shuntVal = monShuntVal(index);
    d->busNomVoltage = monVoltageNom(index);
    d->label = monLabel(index);
    struct_pm_sensor_clear_measurements(d);
}

SensorStatus pm_sensor_status(const pm_sensor d)
{
//    if (d.status != DEVICE_UNKNOWN) {
//        printf("%s: device unknown\n", d.label);
//        return SENSOR_UNKNOWN;
//    }

    float V = d.busVoltage;
    float VMinWarn = d.busNomVoltage * (1-monVoltageMarginWarn(d.index));
    float VMaxWarn = d.busNomVoltage * (1+monVoltageMarginWarn(d.index));
    float VMinCrit = d.busNomVoltage * (1-monVoltageMarginCrit(d.index));
    float VMaxCrit = d.busNomVoltage * (1+monVoltageMarginCrit(d.index));
    int VNorm = (V > VMinWarn) && (V < VMaxWarn);
    int VWarn = (V > VMinCrit) && (V < VMaxCrit);
    if (VNorm && VWarn)
        return SENSOR_NORMAL;
    if (VWarn)
        return SENSOR_WARNING;
//    int isOn = 1; // FIXME: monIsOn(deviceAddr);
//    return isOn ? inRange2 : 1;
    return SENSOR_CRITICAL;
}


int pm_sensor_isValid(const pm_sensor d)
{
    SensorStatus status = pm_sensor_status(d);
    if (status == SENSOR_NORMAL || status == SENSOR_WARNING)
        return 1;
    else
        return 0;
}

void pm_sensor_set_deviceStatus(pm_sensor *d, DeviceStatus status)
{
    DeviceStatus oldStatus = d->deviceStatus;
    if (oldStatus != status) {
        d->deviceStatus = status;
        d->lastStatusUpdatedTick = HAL_GetTick();
    }
}

void pm_sensor_set_sensorStatus(pm_sensor *d, SensorStatus status)
{
    SensorStatus oldStatus = d->sensorStatus;
    if (oldStatus != status) {
        d->sensorStatus = status;
        d->lastStatusUpdatedTick = HAL_GetTick();
    }
}

uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor d)
{
    return HAL_GetTick() - d.lastStatusUpdatedTick;
}

void pm_sensor_print(const pm_sensor d, int isOn)
{
    printf("%6s: ", d.label);
    if (d.deviceStatus == DEVICE_UNKNOWN) {
        printf("DEVICE_UNKNOWN");
    }
    if (d.deviceStatus == DEVICE_FAIL) {
        printf("DEVICE_FAIL");
    }
    const int fractdigits = 3;
    char str1[10], str3[10];
    //    char str2[10];
    ftoa(d.busVoltage, str1, fractdigits);
    //            float marginLo1 = d.busNomVoltage * (1-TOLERANCE_1);
    //            float marginHi1 = d.busNomVoltage * (1+TOLERANCE_1);
    //            float marginLo2 = d.busNomVoltage * (1-TOLERANCE_CRIT);
    //            float marginHi2 = d.busNomVoltage * (1+TOLERANCE_CRIT);
    //            int inRange1 = (d.busVoltage > marginLo1) && (d.busVoltage < marginHi1);
    //            int inRange2 = (d.busVoltage > marginLo2) && (d.busVoltage < marginHi2);
    //            int isOn = 1; // FIXME: monIsOn(deviceAddr);
    //            printf("%s%8s%s", inRange1 ? ANSI_GREEN : inRange2 ? ANSI_YELLOW : isOn ? ANSI_RED : ANSI_GRAY, str1, ANSI_CLEAR);
    SensorStatus status = pm_sensor_status(d);
    const char *color = "";
    switch (status) {
    case SENSOR_NORMAL: color = ANSI_GREEN; break;
    case SENSOR_WARNING: color = ANSI_YELLOW; break;
    case SENSOR_CRITICAL: color = ANSI_RED; break;
    }
    printf("%s%8s%s", color, str1, ANSI_CLEAR);
    if (d.shuntVal > 0) {
        //        ftoa(shunt * 1e3, str2, fractdigits); // mV
        ftoa(d.shuntVoltage / d.shuntVal, str3, fractdigits);
        printf(" %8s", str3);
    } else {
        printf("         ");
    }

    printf(" %s   %ld\n", isOn ? (pm_sensor_isValid(d) ? STR_RESULT_NORMAL : STR_RESULT_FAIL) : STR_RESULT_OFF,
           pm_sensor_get_sensorStatus_Duration(d) / getTickFreqHz());
}

int pm_sensor_detect(pm_sensor *d)
{
    d->lastStatusUpdatedTick = HAL_GetTick();
    uint16_t deviceAddr = d->busAddress;
    uint16_t manuf_id;
    uint16_t device_id;
    int detected =(
                (HAL_OK == ina226_i2c_Detect(deviceAddr))
                && (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_MANUFACTURER_ID, &manuf_id))
                && (manuf_id == INA226_MANUFACTURER_ID)
                && (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_DEVICE_ID, &device_id))
                && (device_id == INA226_DEVICE_ID)
            );
    pm_sensor_set_deviceStatus(d, detected ? DEVICE_NORMAL : DEVICE_FAIL);
    return detected;
}

int pm_sensor_read(pm_sensor *d)
{
    int err = 0;
    struct_pm_sensor_clear_measurements(d);
    uint16_t deviceAddr = d->busAddress;
    uint16_t data;
    if (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_BUS_VOLT, &data)) {
        d->busVoltage = (int16_t)data * 1.25e-3;
    } else {
        err++;
    }
    if (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_SHUNT_VOLT, &data)) {
        d->shuntVoltage = (int16_t)data * 2.5e-6;
    } else {
        err++;
    }
    if (err) {
        pm_sensor_set_deviceStatus(d, err ? DEVICE_FAIL : DEVICE_NORMAL);
        pm_sensor_set_sensorStatus(d, SENSOR_NORMAL);
    } else {
        pm_sensor_set_sensorStatus(d, pm_sensor_status(*d));
    }
    //        printMonValue(deviceAddr, busVolt * 1.25e-3, shuntVolt * 2.5e-6, monShuntVal[i]);
    return err;
}
