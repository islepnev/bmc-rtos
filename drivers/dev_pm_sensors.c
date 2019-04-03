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
#include "ina226_i2c_hal.h"
#include "i2c.h"

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
}

void struct_pm_sensor_clear_measurements(pm_sensor *d)
{
    d->busVoltage = 0;
    d->current = 0;
}

void struct_pm_sensor_init(pm_sensor *d, SensorIndex index)
{
    d->index = index;
    d->deviceStatus = DEVICE_UNKNOWN;
    d->sensorStatus = SENSOR_CRITICAL;
    d->rampState = RAMP_NONE;
    d->lastStatusUpdatedTick = 0;
    d->busAddress = sensorBusAddress(index);
    d->isOptional = monIsOptional(index);
    d->hasShunt = monShuntVal(index) > 1e-6;
    d->shuntVal = monShuntVal(index);
    d->busNomVoltage = monVoltageNom(index);
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
configreg_t default_configreg = {
    .bit.mode = 7,     // 7: Shunt and Bus, Continuous
    .bit.vshct = 4,    // 4: 1 ms
    .bit.vbusct = 4,   // 4: 1 ms
    .bit.avg = 1,      // 1: 4 averages
    .bit.reserved = 4, // should be 4
    .bit.rst = 0       // 0: no reset
};

static HAL_StatusTypeDef pm_sensor_write_conf(pm_sensor *d)
{
    uint16_t deviceAddr = d->busAddress;
    uint16_t data;
    data = default_configreg.raw;
    HAL_StatusTypeDef ret = ina226_i2c_Write(deviceAddr, INA226_REG_CONFIG, data);
    return ret;
}

DeviceStatus pm_sensor_detect(pm_sensor *d)
{
    d->lastStatusUpdatedTick = osKernelSysTick();
    uint16_t deviceAddr = d->busAddress;
    uint16_t manuf_id;
    uint16_t device_id;
    int detected =(
                // (HAL_OK == ina226_i2c_Detect(deviceAddr))
                (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_MANUFACTURER_ID, &manuf_id))
                && (manuf_id == INA226_MANUFACTURER_ID)
                && (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_DEVICE_ID, &device_id))
                && (device_id == INA226_DEVICE_ID)
                && (HAL_OK == pm_sensor_write_conf(d))
            );
    pm_sensor_set_deviceStatus(d, detected ? DEVICE_NORMAL : DEVICE_FAIL);
    return d->deviceStatus;
}

DeviceStatus pm_sensor_read(pm_sensor *d)
{
    uint16_t deviceAddr = d->busAddress;
    uint16_t rawVoltage = 0;
    uint16_t rawCurrent = 0;
    configreg_t configreg = {0};
    int err = 0;
    while (1) {
        if ((HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_BUS_VOLT, &rawVoltage))
                && (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_SHUNT_VOLT, &rawCurrent))
                && (HAL_OK == ina226_i2c_Read(deviceAddr, INA226_REG_CONFIG, &configreg.raw))
                && (configreg.raw == default_configreg.raw)) {
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
    pm_sensor_set_readVoltage(d, (int16_t)rawVoltage * 1.25e-3f);
    const double shuntVoltage = (int16_t)rawCurrent * 2.5e-6f;
    double readCurrent = 0;
    if (d->shuntVal > SENSOR_MINIMAL_SHUNT_VAL)
        readCurrent = shuntVoltage / d->shuntVal;
    pm_sensor_set_readCurrent(d, readCurrent);
    pm_sensor_set_sensorStatus(d, pm_sensor_status(d));
    return d->deviceStatus;
}
