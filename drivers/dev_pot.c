/*
**    Digital Potentiometers
**
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
#include "dev_pot.h"

#include <stdio.h>

#include "ad5141_i2c_hal.h"
#include "powermon_i2c_driver.h"

const char *potLabel(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return "TDC_A";
    case POT_TDC_B: return "TDC_B";
    case POT_TDC_C: return "TDC_C";
    }
    return 0;
}

static int potBusAddress(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return 0x20;
    case POT_TDC_B: return 0x23;
    case POT_TDC_C: return 0x2C;
    }
    return 0;
}

static int potSensorIndex(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return SENSOR_TDC_A;
    case POT_TDC_B: return SENSOR_TDC_B;
    case POT_TDC_C: return SENSOR_TDC_C;
    }
    return 0;
}

void struct_pots_init(Dev_pots *d)
{
    for (int i=0; i<DEV_POT_COUNT; i++) {
        Dev_ad5141 zz = {0};
        d->pot[i] = zz;
        d->pot[i].index = i;
        d->pot[i].deviceStatus = DEVICE_UNKNOWN;
        d->pot[i].sensorIndex = potSensorIndex(i);
        d->pot[i].busAddress = potBusAddress(i);
    }
}

void dev_ad5141_reset(Dev_ad5141 *d)
{
    if (d->deviceStatus == DEVICE_NORMAL)
        ad5141_reset(d->busAddress);
}

void dev_ad5141_inc(Dev_ad5141 *d)
{
    if (d->deviceStatus == DEVICE_NORMAL)
        ad5141_inc_rdac(d->busAddress);
}

void dev_ad5141_dec(Dev_ad5141 *d)
{
    if (d->deviceStatus == DEVICE_NORMAL)
        ad5141_dec_rdac(d->busAddress);
}

void dev_ad5141_write(Dev_ad5141 *d)
{
    if (d->deviceStatus == DEVICE_NORMAL)
        ad5141_copy_rdac_to_eeprom(d->busAddress);
}

static DeviceStatus dev_ad5141_detect(Dev_ad5141 *d)
{
    int detected = (HAL_OK == ad5141_nop(d->busAddress));
    if (detected)
        d->deviceStatus = DEVICE_NORMAL;
    else
        d->deviceStatus = DEVICE_UNKNOWN;
    return d->deviceStatus;
}

int pot_detect(Dev_pots *d)
{
    int count = 0;
    for (int i=0; i<DEV_POT_COUNT; i++) {
        powermon_i2c_reset_master();
        DeviceStatus s = dev_ad5141_detect(&d->pot[i]);
        if (s == DEVICE_NORMAL) {
            dev_ad5141_reset(&d->pot[i]);
            count++;
        } else {
            powermon_i2c_reset_master();
        }
    }
    return count;
}

void pot_read_rdac_all(Dev_pots *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int i=0; i<DEV_POT_COUNT; i++) {
        Dev_ad5141 *p = &d->pot[i];
        if (p->deviceStatus != DEVICE_NORMAL)
            continue;
        ret = ad5141_read_rdac(p->busAddress, &p->value);
        if (HAL_OK != ret)
            break;
    }
}
