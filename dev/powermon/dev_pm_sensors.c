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

#include "dev_pm_sensors.h"

#include <assert.h>

#include "dev_pm_sensors_types.h"
#include "ina226/ina226_i2c_hal.h"

#include "ansi_escape_codes.h"
#include "display.h"
#include "logbuffer.h"
#include "device_status_log.h"
#include "dev_mcu.h"
#include "cmsis_os.h"

//const double SENSOR_VOLTAGE_MARGIN_WARN = 0.05;
const double SENSOR_VOLTAGE_MARGIN_CRIT = 0.1;
const int ERROR_COUNT_LIMIT = 3;

void struct_pm_sensor_clear_minmax(pm_sensor_priv *d)
{
    d->busVoltageMin = 0;
    d->busVoltageMax = 0;
    d->currentMin = 0;
    d->currentMax = 0;
    d->powerMax = 0;
}

void struct_pm_sensor_clear_measurements(pm_sensor_priv *d)
{
    d->valid = false;
    d->busVoltage = 0;
    d->current = 0;
    d->power = 0;
}

void struct_pm_sensor_init(pm_sensor *d, SensorIndex index)
{
    d->dev.device_status = DEVICE_UNKNOWN;
    pm_sensor_priv *sensor = &d->priv;
    sensor->index = index;
    sensor->isOptional = monIsOptional(index);
    sensor->hasShunt = monShuntVal(index) > 1e-6;
    sensor->shuntVal = monShuntVal(index);
    sensor->busNomVoltage = monVoltageNom(index);
    sensor->label = monLabel(index);

    struct_pm_sensor_clear(d);
}

void struct_pm_sensor_clear(pm_sensor *d)
{
    pm_sensor_priv *sensor = &d->priv;
    sensor->rampState = RAMP_NONE;
    sensor->lastStatusUpdatedTick = 0;
    const double current_max = 16; // amperers
    sensor->current_lsb = current_max / 32768.0;
    sensor->cal = 0.00512 / (sensor->current_lsb * sensor->shuntVal);
    struct_pm_sensor_clear_measurements(&d->priv);
    struct_pm_sensor_clear_minmax(&d->priv);
}

static void pm_sensor_set_deviceStatus(pm_sensor *d, DeviceStatus status)
{
    DeviceStatus oldStatus = d->dev.device_status;
    if (oldStatus != status) {
        d->dev.device_status = status;
        d->priv.lastStatusUpdatedTick = osKernelSysTick();
        if (status != DEVICE_NORMAL)
            dev_log_status_change(&d->dev);
    }
}

static void pm_sensor_set_readVoltage(pm_sensor_priv *d, double value)
{
    d->busVoltage = value;
    if (value > d->busVoltageMax)
        d->busVoltageMax = value;
    if (value < d->busVoltageMin)
        d->busVoltageMin = value;
}

static void pm_sensor_set_readCurrent(pm_sensor_priv *d, double value)
{
    d->current = value;
    if (value > d->currentMax)
        d->currentMax = value;
    if (value < d->currentMin)
        d->currentMin = value;
}

static void pm_sensor_set_readPower(pm_sensor_priv *d, double value)
{
    d->power = value;
    if (value > d->powerMax)
        d->powerMax = value;
}

void pm_sensor_set_sensorStatus(pm_sensor *d, SensorStatus status)
{
    SensorStatus oldStatus = d->dev.sensor;
    if (oldStatus != status) {
        if (false)
            sensor_log_status_change(&d->dev);
        d->dev.sensor = status;
        d->priv.lastStatusUpdatedTick = osKernelSysTick();
    }
}

uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor_priv *d)
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
    uint16_t data;
    data = default_configreg().raw;
    return ina226_i2c_Write(&d->dev.bus, INA226_REG_CONFIG, data)
           && ina226_i2c_Write(&d->dev.bus, INA226_REG_CAL, d->priv.cal);
}

DeviceStatus pm_sensor_detect(pm_sensor *d)
{
    d->priv.lastStatusUpdatedTick = osKernelSysTick();
    uint16_t manuf_id;
    uint16_t device_id;
    int detected =(
        // ina226_i2c_Detect(&d->bus)
        ina226_i2c_Read(&d->dev.bus, INA226_REG_MANUFACTURER_ID, &manuf_id)
        && (manuf_id == INA226_MANUFACTURER_ID)
        && ina226_i2c_Read(&d->dev.bus, INA226_REG_DEVICE_ID, &device_id)
        && (device_id == INA226_DEVICE_ID)
        && pm_sensor_write_conf(d)
        );
    pm_sensor_set_deviceStatus(d, detected ? DEVICE_NORMAL : DEVICE_FAIL);
    return d->dev.device_status;
}

SensorStatus pm_sensor_compute_status(const pm_sensor *d)
{
    if (d->dev.device_status != DEVICE_NORMAL ||
        !d->priv.valid) {
        return SENSOR_UNKNOWN;
    }
    const pm_sensor_priv *sensor = &d->priv;
    double V = d->priv.busVoltage;
    double VMinWarn = sensor->busNomVoltage * (1-monVoltageMarginWarn(sensor->index));
    double VMaxWarn = sensor->busNomVoltage * (1+monVoltageMarginWarn(sensor->index));
    double VMinCrit = sensor->busNomVoltage * (1-monVoltageMarginCrit(sensor->index));
    double VMaxCrit = sensor->busNomVoltage * (1+monVoltageMarginCrit(sensor->index));
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
//    uint16_t rawMask = 0;
    uint16_t rawVoltage = 0;
    uint16_t rawShuntVoltage = 0;
#if INA226_USE_INTERNAL_CALC
    uint16_t rawCurrent = 0;
    uint16_t rawPower = 0;
#endif
    configreg_t configreg = {{0}};
    int err = 0;
    while (1) {
        if (err > 0)
            log_printf(LOG_WARNING, "Sensor %s read error, retry %d", d->priv.label, err);
        if (1
//                && ina226_i2c_Read(&d->bus, INA226_REG_MASK, &rawMask)
//                && (rawMask & 0x0400)
            && ina226_i2c_Read(&d->dev.bus, INA226_REG_BUS_VOLT, &rawVoltage)
                && ina226_i2c_Read(&d->dev.bus, INA226_REG_SHUNT_VOLT, &rawShuntVoltage)
#if INA226_USE_INTERNAL_CALC
                && ina226_i2c_Read(&d->dev.bus, INA226_REG_CURRENT, &rawCurrent)
                && ina226_i2c_Read(&d->dev.bus, INA226_REG_POWER, &rawPower)
#endif
                && ina226_i2c_Read(&d->dev.bus, INA226_REG_CONFIG, &configreg.raw)
                && (configreg.raw == default_configreg().raw)) {
            break;
        }
        err++;
        if (err >= ERROR_COUNT_LIMIT) {
            pm_sensor_set_deviceStatus(d, DEVICE_FAIL);
            pm_sensor_set_sensorStatus(d, SENSOR_UNKNOWN);
            struct_pm_sensor_clear_measurements(&d->priv);
            return d->dev.device_status;
        }
    }
    const double readVoltage  = (int16_t)rawVoltage * 1.25e-3;
    pm_sensor_set_readVoltage(&d->priv, readVoltage);
#if INA226_USE_INTERNAL_CALC
    pm_sensor_set_readCurrent(d, d->current_lsb * (int16_t)rawCurrent);
    pm_sensor_set_readPower(d, (int16_t)rawPower * d->current_lsb * 25.0);
#else
    const double shuntVoltage = (int16_t)rawShuntVoltage * 2.5e-6;
    double readCurrent = 0;
    if (d->priv.shuntVal > SENSOR_MINIMAL_SHUNT_VAL)
        readCurrent = shuntVoltage / d->priv.shuntVal;
    pm_sensor_set_readCurrent(&d->priv, readCurrent);
    pm_sensor_set_readPower(&d->priv, readCurrent * readVoltage);
#endif
    d->priv.valid = true;
    pm_sensor_set_sensorStatus(d, pm_sensor_compute_status(d));
    return d->dev.device_status;
}

double get_sensor_power_w(const pm_sensor *d)
{
    SensorStatus sensor_status = pm_sensor_status(d);
    int sensor_present = ((sensor_status == SENSOR_NORMAL) || (sensor_status == SENSOR_WARNING));
    if (sensor_present)
        return d->priv.busVoltage * d->priv.current;
    else
        return 0;
}
