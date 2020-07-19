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

#include "devices.h"
#include "bsp_pin_defs.h"
#include "pca9548_i2c_hal.h"
#include "dev_pll.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "i2c.h"
#include "display.h"

void struct_pca9548_init(Dev_pca9548 *d)
{
    d->present = DEVICE_UNKNOWN;
}

void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

//void struct_ad9545_init(Dev_ad9545 *d)
//{
//    d->fsm_state = PLL_STATE_INIT;
//    d->present = DEVICE_UNKNOWN;
//}

void struct_Devices_init(Devices *d)
{
    struct_dev_leds_init(&d->leds);
    struct_pca9548_init(&d->i2cmux);
    struct_at24c_init(&d->eeprom_config);
    struct_at24c_init(&d->eeprom_vxspb);
}

static DeviceStatus dev_i2cmux_detect(Dev_pca9548 *d)
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
    uint8_t data = 0;
    if (HAL_OK == pca9548_read(&data))
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
    return d->present;
}

static DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    if (HAL_OK == at24c_detect())
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromConfig_Read(0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

static DeviceStatus dev_eepromVxsPb_detect(Dev_at24c *d)
{
    if (HAL_OK == dev_eepromVxsPb_Detect())
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;

//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromVxsPb_Read(0, &data)) {
//        d->present = 1;
//    }
    return d->present;
}

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if (1
//            && (d->i2cmux.present == DEVICE_NORMAL)
            && (d->eeprom_config.present == DEVICE_NORMAL)
//            && (d->eeprom_vxspb.present == DEVICE_NORMAL)
//            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

DeviceStatus devDetect(Devices *d)
{
//    dev_i2cmux_detect(&d->i2cmux);
    dev_eepromConfig_detect(&d->eeprom_config);
//    dev_eepromVxsPb_detect(&d->eeprom_vxspb);
    return getDeviceStatus(d);
}

DeviceStatus devRun(Devices *d)
{
//    pllRun(&d->pll); // FIXME
    return getDeviceStatus(d);
}

PgoodState dev_readPgood(Dev_powermon *pm)
{
    pm_read_pgood(pm);
    return get_all_pgood(pm);
}
