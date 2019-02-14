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

#include "dev_powermon.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "ftoa.h"
#include "ina226_i2c_hal.h"

const int monAddr[POWERMON_SENSORS] = {
    0x43, 0x45, 0x42, 0x44,
    0x40, 0x46, 0x47, 0x48,
    0x4A, 0x4B, 0x4C, 0x4D,
    0x4E};


float monShuntVal(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return 0.002;
    case 0x42: return 0;
    case 0x43: return 0.010;
    case 0x44: return 0;
    case 0x45: return 0.004;
    case 0x46: return 0.002;
    case 0x47: return 0.002;
    case 0x48: return 0.002;
    case 0x4A: return 0.002;
    case 0x4B: return 0.002;
    case 0x4C: return 0.002;
    case 0x4D: return 0.002;
    case 0x4E: return 0.002;
    default: return 0;
    }
}

float monBusNom(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return 1.5;
    case 0x42: return 5.0;
    case 0x43: return 5.0;
    case 0x44: return 3.3;
    case 0x45: return 3.3;
    case 0x46: return 1.0;
    case 0x47: return 1.0;
    case 0x48: return 1.2;
    case 0x4A: return 1.8;
    case 0x4B: return 2.5;
    case 0x4C: return 2.5;
    case 0x4D: return 2.5;
    case 0x4E: return 2.5;
    default: return 0;
    }
}

const char *monLabel(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return "  int 1.5V"; // U1, ? mOhm
    case 0x42: return "        5V"; // U3
    case 0x43: return "    VME 5V"; // U5, 10 mOhm
    case 0x44: return "      3.3V"; // U7
    case 0x45: return "  VME 3.3V"; // U9, 4 mOhm
    case 0x46: return "  FPGA 1.0"; // U11, 2 mOhm
    case 0x47: return "   MGT 1.0"; // U13, 2 mOhm
    case 0x48: return "   MGT 1.2"; // U2, 2 mOhm
    case 0x4A: return "  FPGA 1.8"; // U, 2 mOhm
    case 0x4B: return " TDC-A 2.5"; // U, 2 mOhm
    case 0x4C: return " TDC-B 2.5"; // U, 2 mOhm
    case 0x4D: return " TDC-C 2.5"; // U, 2 mOhm
    case 0x4E: return " Clock 2.5"; // U, 2 mOhm
    default: return "???";
    }
}

const float TOLERANCE_1 = 0.05;
const float TOLERANCE_CRIT = 0.1;

//float monValuesBus[POWERMON_SENSORS];
//float monValuesShunt[POWERMON_SENSORS];

SensorStatus pm_sensor_status(const pm_sensor d)
{
//    if (d.status != DEVICE_UNKNOWN) {
//        printf("%s: device unknown\n", d.label);
//        return SENSOR_UNKNOWN;
//    }

    float bus = d.busVoltage;
    float marginLo1 = d.busNomVoltage * (1-TOLERANCE_1);
    float marginHi1 = d.busNomVoltage * (1+TOLERANCE_1);
    float marginLo2 = d.busNomVoltage * (1-TOLERANCE_CRIT);
    float marginHi2 = d.busNomVoltage * (1+TOLERANCE_CRIT);
    int inRange1 = (bus > marginLo1) && (bus < marginHi1);
    int inRange2 = (bus > marginLo2) && (bus < marginHi2);
    if (inRange1 && inRange2)
        return SENSOR_NORMAL;
    if (inRange2)
        return SENSOR_WARNING;
//    int isOn = 1; // FIXME: monIsOn(deviceAddr);
//    return isOn ? inRange2 : 1;
    return SENSOR_CRITICAL;
}

void struct_pm_sensor_clear_measurements(pm_sensor *d)
{
    d->busVoltage = 0;
    d->shuntVoltage = 0;
    d->current = 0;
}

void struct_pm_sensor_init(pm_sensor *d, int busAddress)
{
    d->status = DEVICE_UNKNOWN;
    d->busAddress = busAddress;
    d->hasShunt = monShuntVal(busAddress) > 1e-6;
    d->shuntVal = monShuntVal(busAddress);
    d->busNomVoltage = monBusNom(busAddress);
    d->label = monLabel(busAddress);
    struct_pm_sensor_clear_measurements(d);
}

void struct_powermon_init(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_init(&d->sensors[i], monAddr[i]);
    }
//    d->present = DEVICE_UNKNOWN;
}


int pm_sensor_isValid(const pm_sensor d)
{
    SensorStatus status = pm_sensor_status(d);
    if (status == SENSOR_NORMAL || status == SENSOR_WARNING)
        return 1;
    else
        return 0;
}

int getPowerMonState(const Dev_powermon d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(d.sensors[i]))
            return 0;
    return 1;
}

void pm_sensor_print(const pm_sensor d)
{
    printf("%6s: ", d.label);
    if (d.status == DEVICE_UNKNOWN) {
        printf("DEVICE_UNKNOWN");
    }
    if (d.status == DEVICE_FAIL) {
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
    case SENSOR_UNKNOWN: break;
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

    printf(" %s\n", pm_sensor_isValid(d) ? STR_NORMAL : STR_FAIL);
}

int monDetect(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        d->sensors[i].status = DEVICE_UNKNOWN;
    }
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        if (err > 2)
            break;
        int detected = 1;
        uint16_t deviceAddr = d->sensors[i].busAddress;
        uint16_t data;
        if (HAL_OK != ina226_i2c_Read(deviceAddr, INA226_REG_MANUFACTURER_ID, &data)) {
            detected = 0;
        }
        if (data != INA226_MANUFACTURER_ID) {
            detected = 0;
        }
        if (HAL_OK != ina226_i2c_Read(deviceAddr, INA226_REG_DEVICE_ID, &data)) {
            detected = 0;
        }
        if (data != INA226_DEVICE_ID) {
            detected = 0;
        }
        d->sensors[i].status = detected ? DEVICE_NORMAL : DEVICE_FAIL;
        if (!detected)
            err++;
    }
    return err;
}

int monReadValues(Dev_powermon *d)
{
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_measurements(&d->sensors[i]);
        uint16_t deviceAddr = d->sensors[i].busAddress;
        uint16_t data;
        if (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_BUS_VOLT, &data)) {
            d->sensors[i].busVoltage = (int16_t)data * 1.25e-3;
        } else {
            err++;
        }
        if (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_SHUNT_VOLT, &data)) {
            d->sensors[i].shuntVoltage = (int16_t)data * 2.5e-6;
        } else {
            err++;
        }
//        printMonValue(deviceAddr, busVolt * 1.25e-3, shuntVolt * 2.5e-6, monShuntVal[i]);
    }
    return err;
}
