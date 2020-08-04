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

#include "ina226.h"

#include "cmsis_os.h"
#include "ina226/ina226_i2c_hal.h"

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
} ina226_configreg_t;

// default value 0x4127
static ina226_configreg_t ina226_default_configreg(void)
{
    ina226_configreg_t r;
    r.bit.mode = 7;     // 7: Shunt and Bus, Continuous
    r.bit.vshct = 4;    // 4: 1 ms
    r.bit.vbusct = 4;   // 4: 1 ms
    r.bit.avg = 1;      // 1: 4 averages
    r.bit.reserved = 4; // should be 4
    r.bit.rst = 0 ;      // 0: no reset
    return r;
}

bool ina226_write_conf(BusInterface *bus, uint16_t cal)
{
    uint16_t data;
    data = ina226_default_configreg().raw;
    return ina226_i2c_Write(bus, INA226_REG_CONFIG, data)
           && ina226_i2c_Write(bus, INA226_REG_CAL, cal);
}

bool ina226_detect(BusInterface *bus)
{
    uint16_t manuf_id;
    uint16_t device_id;
    return
        // ina226_i2c_Detect(bus)
        ina226_i2c_Read(bus, INA226_REG_MANUFACTURER_ID, &manuf_id)
        && (manuf_id == INA226_MANUFACTURER_ID)
        && ina226_i2c_Read(bus, INA226_REG_DEVICE_ID, &device_id)
        && (device_id == INA226_DEVICE_ID);
}

bool ina226_read(BusInterface *bus, ina226_rawdata_t *rawdata)
{
    //    uint16_t rawMask = 0;
    ina226_configreg_t configreg = {{0}};
    return 1
            //                && ina226_i2c_Read(&d->bus, INA226_REG_MASK, &rawMask)
            //                && (rawMask & 0x0400)
           && ina226_i2c_Read(bus, INA226_REG_BUS_VOLT, &rawdata->rawVoltage)
           && ina226_i2c_Read(bus, INA226_REG_SHUNT_VOLT, &rawdata->rawShuntVoltage)
#if INA226_USE_INTERNAL_CALC
            && ina226_i2c_Read(bus, INA226_REG_CURRENT, &rawdata->rawCurrent)
            && ina226_i2c_Read(bus, INA226_REG_POWER, &rawdata->rawPower)
#endif
            && ina226_i2c_Read(bus, INA226_REG_CONFIG, &configreg.raw)
            && (configreg.raw == ina226_default_configreg().raw);
}
