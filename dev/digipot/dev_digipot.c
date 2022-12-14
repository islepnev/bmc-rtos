/*
**    Digital Potentiometers
**
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

#include "dev_digipot.h"

#include <stdio.h>

#include "ad5141/ad5141.h"
#include "ad5141/ad5141_i2c_hal.h"

bool dev_ad5141_reset(Dev_ad5141 *d)
{
    if (d->dev.device_status == DEVICE_NORMAL)
        return ad5141_reset(&d->dev.bus);
    return false;
}

bool dev_ad5141_inc(Dev_ad5141 *d)
{
    if (d->dev.device_status == DEVICE_NORMAL)
        return ad5141_inc_rdac(&d->dev.bus);
    return false;
}

bool dev_ad5141_dec(Dev_ad5141 *d)
{
    if (d->dev.device_status == DEVICE_NORMAL)
        return ad5141_dec_rdac(&d->dev.bus);
    return false;
}

bool dev_ad5141_write(Dev_ad5141 *d)
{
    if (d->dev.device_status == DEVICE_NORMAL)
        return ad5141_copy_rdac_to_eeprom(&d->dev.bus);
    return false;
}

static DeviceStatus dev_ad5141_detect(Dev_ad5141 *d)
{
    bool detected = ad5141_detect(&d->dev.bus) && ad5141_nop(&d->dev.bus);
    d->dev.device_status = detected ? DEVICE_NORMAL : DEVICE_UNKNOWN;
    return d->dev.device_status;
}

int digipot_detect(Dev_digipots *d)
{
    int count = 0;
    for (uint i=0; i<d->priv.count; i++) {
        DeviceStatus s = dev_ad5141_detect(&d->priv.pot[i]);
        if (s == DEVICE_NORMAL) {
            dev_ad5141_reset(&d->priv.pot[i]);
            count++;
        }
    }
    return count;
}

bool digipot_read_rdac_all(Dev_digipots *d)
{
    for (uint i=0; i<d->priv.count; i++) {
        Dev_ad5141 *p = &d->priv.pot[i];
        if (p->dev.device_status != DEVICE_NORMAL)
            continue;
        if (! ad5141_read_rdac(&p->dev.bus, &p->priv.value))
            return false;
    }
    return true;
}
