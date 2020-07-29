/*
**    Copyright 2019-2020 Ilja Slepnev
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


#include "dev_tmp421.h"

#include "devices_types.h"
#include "i2c.h"
#include "tmp421/tmp421_i2c_hal.h"

enum {
    TMP421_REG_LOCAL_TEMP_HI = 0x00,
    TMP421_REG_REMOTE_TEMP_1_HI = 0x01,
    TMP421_REG_REMOTE_TEMP_2_HI = 0x02,
    TMP421_REG_REMOTE_TEMP_3_HI = 0x03,
    TMP421_REG_STATUS = 0x08,
    TMP421_REG_CONFIG_1 = 0x09,
    TMP421_REG_CONFIG_2 = 0x0A,
    TMP421_REG_CONV_RATE = 0x0B,
    TMP421_REG_ONESHOT_START = 0x0F,
    TMP421_REG_LOCAL_TEMP_LO = 0x10,
    TMP421_REG_REMOTE_TEMP_1_LO = 0x11,
    TMP421_REG_REMOTE_TEMP_2_LO = 0x12,
    TMP421_REG_REMOTE_TEMP_3_LO = 0x13,
    TMP421_REG_NC_1 = 0x21,
    TMP421_REG_NC_2 = 0x22,
    TMP421_REG_NC_3 = 0x23,
    TMP421_REG_SOFTRESET = 0xFC,
    TMP421_REG_MANUF_ID = 0xFE,
    TMP421_REG_DEVICE_ID = 0xFF
};

enum {
    TMP421_MANUF_ID = 0x55
};

enum {
    TMP421_DEVICE_ID = 0x21,
    TMP422_DEVICE_ID = 0x22,
    TMP423_DEVICE_ID = 0x23
};

bool dev_tmp421_detect(Dev_tmp421 *d)
{
    uint8_t data;
    if (tmp421_read(&d->dev.bus, TMP421_REG_MANUF_ID, &data) &&
        (data == TMP421_MANUF_ID) &&
        tmp421_read(&d->dev.bus, TMP421_REG_DEVICE_ID, &data) &&
        (data == TMP421_DEVICE_ID)
        )
        return true;
    return false;
}

bool dev_tmp421_read(Dev_tmp421 *d)
{
    uint8_t data1;
    uint8_t data2;
    if (! tmp421_read(&d->dev.bus, TMP421_REG_REMOTE_TEMP_1_HI, &data1) ||
        ! tmp421_read(&d->dev.bus, TMP421_REG_REMOTE_TEMP_1_LO, &data2)
        ) {
        return false;
    }
    d->priv.rawTemp = (uint16_t)data1 << 8 | data2;
    d->priv.temp = 1.*d->priv.rawTemp/256;
    return true;
}
