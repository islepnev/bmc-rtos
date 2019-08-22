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
#include "dev_pot.h"
#include "ad5141_i2c_hal.h"
#include "bsp.h"
#include "logbuffer.h"

typedef enum {
    POT_TDC_A,
    POT_TDC_B,
    POT_TDC_C,
} PotIndex;

static int potBusAddress(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return 0x20;
    case POT_TDC_B: return 0x23;
    case POT_TDC_C: return 0x2C;
    }
    return 0;
}

void struct_pots_init(Dev_pots *d)
{
    for (int i=0; i<DEV_POT_COUNT; i++) {
        Dev_ad5141 zz = {0};
        d->pot[i] = zz;
        d->pot[i].busAddress = potBusAddress(i);
    }
}

typedef enum {POT_TEST_OK = 0, POT_TEST_FAIL = -1} pot_test_t;

static pot_test_t pot_i2c_Test(Dev_ad5141 *d)
{
    uint8_t test_wr = 0x77;

    if (HAL_OK != dev_ad5141_nop(d->busAddress))
        return POT_TEST_FAIL;

    if (HAL_OK != dev_ad5141_write_rdac(d->busAddress, test_wr))
        return POT_TEST_FAIL;

    //    ad5141_copy_rdac_to_eeprom(d);
    //    ad5141_copy_eeprom_to_rdac(d);
    uint8_t test_rd = 0;
    if (HAL_OK != dev_ad5141_read_rdac(d->busAddress, &test_rd))
        return POT_TEST_FAIL;
    d->value = test_rd;

//    if (test_wr != test_rd)
//        return POT_TEST_FAIL;
    return POT_TEST_OK;
}

static DeviceStatus dev_ad5141_detect(Dev_ad5141 *d)
{
    int detected = (POT_TEST_OK == pot_i2c_Test(d));
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
        pm_sensor_reset_i2c_master();
        DeviceStatus s = dev_ad5141_detect(&d->pot[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            pm_sensor_reset_i2c_master();
        }
    }
    return count;
}
