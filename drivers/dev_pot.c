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
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_i2c.h"
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

static const int I2C_TIMEOUT_MS = 100;

static HAL_StatusTypeDef pot_i2c_Test(uint16_t deviceAddr)
{
    HAL_StatusTypeDef ret;
    enum {Size = 2};
    uint8_t pData[Size] = {0};
    ret = HAL_I2C_Master_Transmit(hi2c_sensors, deviceAddr << 1, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

static DeviceStatus pm_pot_detect(Dev_ad5141 *d)
{
    uint16_t deviceAddr = d->busAddress;
    int detected = (HAL_OK == pot_i2c_Test(deviceAddr));
    if (detected)
        d->deviceStatus = DEVICE_NORMAL;
    else
        d->deviceStatus = DEVICE_UNKNOWN;
    return d->deviceStatus;
}

int pot_detect(Dev_pots *d)
{
    if (HAL_I2C_STATE_READY != hi2c_sensors->State) {
        log_printf(LOG_ERR, "%s: I2C not ready, state %d", __func__, hi2c_sensors->State);
        return 0;
    }
    int count = 0;
    for (int i=0; i<DEV_POT_COUNT; i++) {
        pm_sensor_reset_i2c_master();
        DeviceStatus s = pm_pot_detect(&d->pot[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            pm_sensor_reset_i2c_master();
        }
    }
    return count;
}
