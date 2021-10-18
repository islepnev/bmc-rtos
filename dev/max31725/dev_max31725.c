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

#include "dev_max31725.h"

#include "log/log.h"
#include "max31725/max31725_i2c_hal.h"


// static const uint16_t MAX31725_REG_THERM_POR  = 0x0000;
static const uint8_t MAX31725_REG_CONFIG_DATA = 0x20;
static const uint8_t MAX31725_REG_CONFIG_POR  = 0x00;
static const uint8_t MAX31725_REG_CONFIG_POR_BUG = 0x20;
static const uint16_t MAX31725_REG_THYST_POR  = 0x4b00;
static const uint16_t MAX31725_REG_TOS_POR    = 0x5000;

bool dev_max31725_write_check(Dev_max31725 *d, max31725_reg_t reg, uint16_t wrdata)
{
    uint16_t rddata = 0;
    if (! max31725_write(&d->dev.bus, reg, wrdata))
        return false;
    if (! max31725_read(&d->dev.bus, reg, &rddata))
        return false;
    if (wrdata != rddata) {
        log_printf(LOG_WARNING, "MAX31725 sensor %s register %d data error: wrote %04X, read %04X",
                   d->dev.name, reg, wrdata, rddata);
        return false;
    }
    return true;
}

bool dev_max31725_detect(Dev_max31725 *d)
{
    if (false && ! max31725_detect(&d->dev.bus))
        return false;

    // check Power-On Reset register values
    {
        uint16_t data;
        if (! max31725_read(&d->dev.bus, MAX31725_REG_THYST, &data))
            return false;
        if (data != MAX31725_REG_THYST_POR) {
            log_printf(LOG_WARNING, "MAX31725 sensor %s unexpected THYST register value: %04X",
                       d->dev.name, data);
            if (!dev_max31725_write_check(d, MAX31725_REG_THYST, MAX31725_REG_THYST_POR))
                return false;
        }
    }
    {
        uint16_t data;
        if (! max31725_read(&d->dev.bus, MAX31725_REG_TOS, &data))
            return false;
        if (data != MAX31725_REG_TOS_POR) {
            log_printf(LOG_WARNING, "MAX31725 sensor %s unexpected TOS register value: %04X",
                       d->dev.name, data);
            if (!dev_max31725_write_check(d, MAX31725_REG_TOS, MAX31725_REG_TOS_POR))
                return false;
        }
    }
    // check
    if (1) {
        uint16_t wrdata = 0x5aa5;
        uint16_t rddata = 0;
        if (! max31725_write(&d->dev.bus, MAX31725_REG_TOS, wrdata))
            return false;
        if (! max31725_read(&d->dev.bus, MAX31725_REG_TOS, &rddata))
            return false;
        if (! max31725_write(&d->dev.bus, MAX31725_REG_TOS, MAX31725_REG_TOS_POR))
            return false;
        if (wrdata != rddata) {
            log_printf(LOG_WARNING, "MAX31725 sensor %s TOS register data error: wrote %04X, read %04X",
                       d->dev.name, wrdata, rddata);
            return false;
        }
    }
    // check power-on config
    uint8_t config;
    {
        if (! max31725_read_config(&d->dev.bus, MAX31725_REG_CONFIG, &config))
            return false;
        if (!d->priv.configured) {
            if (config == MAX31725_REG_CONFIG_POR_BUG) {
                log_printf(LOG_WARNING, "MAX31725 sensor %s CONFIG register power-on value: %02X, enabling workaround",
                           d->dev.name, config);
                d->priv.config_00_workaround = true;
            } else {
                if (config != MAX31725_REG_CONFIG_POR) {
                    log_printf(LOG_WARNING, "MAX31725 sensor %s CONFIG register power-on value: %02X, expected %02X",
                               d->dev.name, config, MAX31725_REG_CONFIG_POR);
                }
            }
        }
    }
    // write config (if required)
    {
        if (config != MAX31725_REG_CONFIG_DATA) {
            uint8_t wrdata = MAX31725_REG_CONFIG_DATA;
            if (! max31725_write_config(&d->dev.bus, MAX31725_REG_CONFIG, wrdata))
                return false;
            d->priv.configured = true;
            uint8_t data;
            if (! max31725_read_config(&d->dev.bus, MAX31725_REG_CONFIG, &data))
                return false;
            if (data != wrdata) {
                log_printf(LOG_WARNING, "MAX31725 sensor %s config write error: wrote %02X, read %02X",
                           d->dev.name, wrdata, data);
                return false;
            } else {
                log_printf(LOG_INFO, "MAX31725 sensor %s config changed %02X to %02X",
                           d->dev.name, config, data);
            }
        }
    }
    return true;
}

bool dev_max31725_read(Dev_max31725 *d)
{
    uint16_t data;
    if (! max31725_read(&d->dev.bus, MAX31725_REG_THERM, &data))
        return false;

    d->priv.temp = (int16_t)data/256.0;
//    log_printf(LOG_INFO, "MAX31725 %s raw read %04X  %d",
//               d->dev.name, data, (int16_t)(data));
    if (MAX31725_REG_CONFIG_DATA & 0x20)
        d->priv.temp += 64.0; // extended temperature format
    return true;
}
