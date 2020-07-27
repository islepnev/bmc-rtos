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

#include "app_shared_data.h"
#include "devices_types.h"
#include "log/log.h"
#include "max31725/max31725_i2c_hal.h"

enum {
    MAX31725_REG_THERM = 0,
    MAX31725_REG_CONFIG = 1,
    MAX31725_REG_THYST = 2,
    MAX31725_REG_TOS = 3
};

// static const uint16_t MAX31725_REG_THERM_POR  = 0x0000;
// static const uint16_t MAX31725_REG_CONFIG_POR = 0x0040;
static const uint16_t MAX31725_REG_THYST_POR  = 0x4b00;
// static const uint16_t MAX31725_REG_TOS_POR    = 0x5000;

bool dev_max31725_detect(Dev_max31725 *d)
{
    uint16_t data;
    if (! max31725_detect(&d->bus) || ! max31725_read(&d->bus, MAX31725_REG_THYST, &data))
        return false;
    if (data != MAX31725_REG_THYST_POR) {
        return false;
    }
    return true;
}

bool dev_max31725_read(Dev_max31725 *d)
{
    uint16_t data;
    if (! max31725_read(&d->bus, 0, &data))
        return false;

    d->temp = (int16_t)data/256.0 + 64.0;
    return true;
}
