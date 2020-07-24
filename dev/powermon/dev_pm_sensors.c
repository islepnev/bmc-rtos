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
#include "dev_pm_sensors_types.h"
#include "ina226/ina226_i2c_hal.h"

#include "ansi_escape_codes.h"
#include "display.h"
#include "logbuffer.h"
#include "dev_mcu.h"
#include "cmsis_os.h"

//const double SENSOR_VOLTAGE_MARGIN_WARN = 0.05;
const double SENSOR_VOLTAGE_MARGIN_CRIT = 0.1;
const int ERROR_COUNT_LIMIT = 3;

void struct_pm_sensor_clear_minmax(pm_sensor *d)
{
    d->busVoltageMin = 0;
    d->busVoltageMax = 0;
    d->currentMin = 0;
    d->currentMax = 0;
    d->powerMax = 0;
}

void struct_pm_sensor_clear_measurements(pm_sensor *d)
{
    d->sensorStatus = SENSOR_UNKNOWN;
    d->busVoltage = 0;
    d->current = 0;
    d->power = 0;
}

void struct_pm_sensor_init(pm_sensor *d, SensorIndex index)
{
    d->index = index;
    d->deviceStatus = DEVICE_UNKNOWN;
    d->sensorStatus = SENSOR_UNKNOWN;
    d->rampState = RAMP_NONE;
    d->lastStatusUpdatedTick = 0;
    d->busAddress = sensorBusAddress(index);
    d->isOptional = monIsOptional(index);
    d->hasShunt = monShuntVal(index) > 1e-6;
    d->shuntVal = monShuntVal(index);
    d->busNomVoltage = monVoltageNom(index);
    const double current_max = 16; // amperers
    d->current_lsb = current_max / 32768.0;
    d->cal = 0.00512 / (d->current_lsb * d->shuntVal);
    d->label = monLabel(index);
    struct_pm_sensor_clear_measurements(d);
    struct_pm_sensor_clear_minmax(d);
}

static void pm_sensor_set_deviceStatus(pm_sensor *d, DeviceStatus status)
{
    DeviceStatus oldStatus = d->deviceStatus;
    if (oldStatus != status) {
        d->deviceStatus = status;
        d->lastStatusUpdatedTick = osKernelSysTick();
    }
}

static void pm_sensor_set_readVoltage(pm_sensor *d, double value)
{
    d->busVoltage = value;
    if (value > d->busVoltageMax)
        d->busVoltageMax = value;
    if (value < d->busVoltageMin)
        d->busVoltageMin = value;
}

static void pm_sensor_set_readCurrent(pm_sensor *d, double value)
{
    d->current = value;
    if (value > d->currentMax)
        d->currentMax = value;
    if (value < d->currentMin)
        d->currentMin = value;
}

static void pm_sensor_set_readPower(pm_sensor *d, double value)
{
    d->power = value;
    if (value > d->powerMax)
        d->powerMax = value;
}

static void pm_sensor_set_sensorStatus(pm_sensor *d, SensorStatus status)
{
    SensorStatus oldStatus = d->sensorStatus;
    if (oldStatus != status) {
        d->sensorStatus = status;
        d->lastStatusUpdatedTick = osKernelSysTick();
    }
}

uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor *d)
{
    return osKernelSysTick() - d->lastStatusUpdatedTick;
}

typedef union {
    struct {
        unsigned int mode:3;
        unsigned int vshct:3;
        unsigned int vbusct:3;
        unsigned int avg:3;
        unsigned int reserved:3;
        unsigned int rst:1;
    } bit;
    uint16_t raw;
} configreg_t;

// default value 0x4127
static configreg_t default_configreg(void)
{
    configreg_t r;
    r.bit.mode = 7;     // 7: Shunt and Bus, Continuous
    r.bit.vshct = 4;    // 4: 1 ms
    r.bit.vbusct = 4;   // 4: 1 ms
    r.bit.avg = 1;      // 1: 4 averages
    r.bit.reserved = 4; // should be 4
    r.bit.rst = 0 ;      // 0: no reset
    return r;
}

static bool pm_sensor_write_conf(pm_sensor *d)
{
    uint16_t deviceAddr = d->busAddress;
    uint16_t data;
    data = default_configreg().raw;
    return ina226_i2c_Write(deviceAddr, INA226_REG_CONFIG, data)
           && ina226_i2c_Write(deviceAddr, INA226_REG_CAL, d->cal);
}

DeviceStatus pm_sensor_detect(pm_sensor *d)
{
    d->lastStatusUpdatedTick = osKernelSysTick();
    uint16_t deviceAddr = d->busAddress;
    uint16_t manuf_id;
    uint16_t device_id;
    int detected =(
                // ina226_i2c_Detect(deviceAddr)
                ina226_i2c_Read(deviceAddr, INA226_REG_MANUFACTURER_ID, &manuf_id)
                && (manuf_id == INA226_MANUFACTURER_ID)
                && ina226_i2c_Read(deviceAddr, INA226_REG_DEVICE_ID, &device_id)
                && (device_id == INA226_DEVICE_ID)
                && pm_sensor_write_conf(d)
            );
    pm_sensor_set_deviceStatus(d, detected ? DEVICE_NORMAL : DEVICE_FAIL);
    return d->deviceStatus;
}

SensorStatus pm_sensor_compute_status(const pm_sensor *d)
{
    if (d->deviceStatus != DEVICE_NORMAL) {
        return SENSOR_UNKNOWN;
    }

    double V = d->busVoltage;
    double VMinWarn = d->busNomVoltage * (1-monVoltageMarginWarn(d->index));
    double VMaxWarn = d->busNomVoltage * (1+monVoltageMarginWarn(d->index));
    double VMinCrit = d->busNomVoltage * (1-monVoltageMarginCrit(d->index));
    double VMaxCrit = d->busNomVoltage * (1+monVoltageMarginCrit(d->index));
    int VNorm = (V > VMinWarn) && (V < VMaxWarn);
    int VWarn = (V > VMinCrit) && (V < VMaxCrit);
    if (VNorm && VWarn) {
        return SENSOR_NORMAL;
    }
    if (VWarn) {
        return SENSOR_WARNING;
    }
    return SENSOR_CRITICAL;
}

#define INA226_USE_INTERNAL_CALC 0

DeviceStatus pm_sensor_read(pm_sensor *d)
{
    uint16_t deviceAddr = d->busAddress;
//    uint16_t rawMask = 0;
    uint16_t rawVoltage = 0;
    uint16_t rawShuntVoltage = 0;
#if INA226_USE_INTERNAL_CALC
    uint16_t rawCurrent = 0;
    uint16_t rawPower = 0;
#endif
    configreg_t configreg = {0};
    int err = 0;
    while (1) {
        if (1
//                && ina226_i2c_Read(deviceAddr, INA226_REG_MASK, &rawMask)
//                && (rawMask & 0x0400)
                && ina226_i2c_Read(deviceAddr, INA226_REG_BUS_VOLT, &rawVoltage)
                && ina226_i2c_Read(deviceAddr, INA226_REG_SHUNT_VOLT, &rawShuntVoltage)
#if INA226_USE_INTERNAL_CALC
                && ina226_i2c_Read(deviceAddr, INA226_REG_CURRENT, &rawCurrent)
                && ina226_i2c_Read(deviceAddr, INA226_REG_POWER, &rawPower)
#endif
                && ina226_i2c_Read(deviceAddr, INA226_REG_CONFIG, &configreg.raw)
                && (configreg.raw == default_configreg().raw)) {
            break;
        }
        err++;
        if (err > ERROR_COUNT_LIMIT) {
            log_printf(LOG_ERR, "Sensor %s failed", d->label);
            pm_sensor_set_deviceStatus(d, DEVICE_FAIL);
            pm_sensor_set_sensorStatus(d, SENSOR_UNKNOWN);
            struct_pm_sensor_clear_measurements(d);
            return d->deviceStatus;
        }
    }
    const double readVoltage  = (int16_t)rawVoltage * 1.25e-3;
    pm_sensor_set_readVoltage(d, readVoltage);
#if INA226_USE_INTERNAL_CALC
    pm_sensor_set_readCurrent(d, d->current_lsb * (int16_t)rawCurrent);
    pm_sensor_set_readPower(d, (int16_t)rawPower * d->current_lsb * 25.0);
#else
    const double shuntVoltage = (int16_t)rawShuntVoltage * 2.5e-6;
    double readCurrent = 0;
    if (d->shuntVal > SENSOR_MINIMAL_SHUNT_VAL)
        readCurrent = shuntVoltage / d->shuntVal;
    pm_sensor_set_readCurrent(d, readCurrent);
    pm_sensor_set_readPower(d, readCurrent * readVoltage);
#endif
    pm_sensor_set_sensorStatus(d, pm_sensor_compute_status(d));
    return d->deviceStatus;
}

double get_sensor_power_w(const pm_sensor *d)
{
    SensorStatus sensor_status = pm_sensor_status(d);
    int sensor_present = ((sensor_status == SENSOR_NORMAL) || (sensor_status == SENSOR_WARNING));
    if (sensor_present)
        return d->busVoltage * d->current;
    else
        return 0;
}
