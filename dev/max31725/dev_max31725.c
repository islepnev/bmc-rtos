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

enum {
    MAX31725_REG_THERM = 0,
    MAX31725_REG_CONFIG = 1,
    MAX31725_REG_THYST = 2,
    MAX31725_REG_TOS = 3
};

// static const uint16_t MAX31725_REG_THERM_POR  = 0x0000;
static const uint8_t MAX31725_REG_CONFIG_DATA = 0x20;
static const uint16_t MAX31725_REG_THYST_POR  = 0x4b00;
// static const uint16_t MAX31725_REG_TOS_POR    = 0x5000;

bool dev_max31725_detect(Dev_max31725 *d)
{
    if (false && ! max31725_detect(&d->dev.bus))
        return false;

    // check Power-On Reset register values
    {    uint16_t data;
        if (! max31725_read(&d->dev.bus, MAX31725_REG_THYST, &data))
            return false;
        if (data != MAX31725_REG_THYST_POR) {
            return false;
        }
    }
    // write config (if required)
    {
        uint8_t pordata;
        if (! max31725_read_config(&d->dev.bus, MAX31725_REG_CONFIG, &pordata))
            return false;
        if (pordata != MAX31725_REG_CONFIG_DATA) {
            uint8_t wrdata = MAX31725_REG_CONFIG_DATA;
            if (! max31725_write_config(&d->dev.bus, MAX31725_REG_CONFIG, wrdata))
                return false;
            uint8_t data;
            if (! max31725_read_config(&d->dev.bus, MAX31725_REG_CONFIG, &data))
                return false;
            if (data != wrdata) {
                log_printf(LOG_WARNING, "MAX31725 sensor %s config write error: wrote %02X, read %02X",
                           d->dev.name, wrdata, data);
                return false;
            } else {
                log_printf(LOG_INFO, "MAX31725 sensor %s config changed %02X to %02X",
                           d->dev.name, pordata, data);
            }
        }
    }

    return true;
}

bool dev_max31725_read(Dev_max31725 *d)
{
    uint16_t data;
    if (! max31725_read(&d->dev.bus, 0, &data))
        return false;

    d->priv.temp = (int16_t)data/256.0;
    if (MAX31725_REG_CONFIG_DATA & 0x20)
        d->priv.temp += 64.0; // extended temperature fomrat
    return true;
}
