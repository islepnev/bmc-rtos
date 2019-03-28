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
#include "adt7301_spi_hal.h"
#include "dev_sfpiic.h"
//#include "dev_vxsiic.h"
//#include "dev_pll.h"
#include "fpga_spi_hal.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "i2c.h"
#include "display.h"

// Temperature limits
static const int tempMinCrit = -40;
static const int tempMaxCrit = 80.0;
static const int tempMinWarn = 0.1;
static const int tempMaxWarn = 60.0;

void struct_thset_init(Dev_thset *d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].valid = 0;
        d->th[i].rawTemp = TEMP_RAW_ERROR;
    }
}

void struct_fpga_init(Dev_fpga *d)
{
    d->present = DEVICE_UNKNOWN;
    d->id = 0;
}

void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

void struct_ad9545_init(Dev_ad9545 *d)
{
    d->fsm_state = PLL_STATE_INIT;
    d->present = DEVICE_UNKNOWN;
}

void struct_Devices_init(Devices *d)
{
    struct_dev_leds_init(&d->leds);
    struct_thset_init(&d->thset);
    struct_fpga_init(&d->fpga);
    struct_at24c_init(&d->eeprom_config);
}

static DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    if (HAL_OK == dev_eepromConfig_Detect())
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromConfig_Read(0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

static DeviceStatus fpgaDetect(Dev_fpga *d)
{
    uint16_t data[2] = {0,0};
    int err = 0;
    for (int i=0; i<2; i++) {
        if (HAL_OK != fpga_spi_hal_read_reg(i, &data[i])) {
            err++;
            break;
        }
    }
    uint8_t id = data[0] & 0xFF;
    if (id == 0x00 || id == 0xFF)
        err++;
    if (err == 0)
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;

    d->id = id;
    return d->present;
}
DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if ((d->sfpiic.present == DEVICE_NORMAL)
            && (d->vxsiic.present == DEVICE_NORMAL)
//            && (d->eeprom_config.present == DEVICE_NORMAL)
//            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

DeviceStatus devDetect(Devices *d)
{
    dev_eepromConfig_detect(&d->eeprom_config);
//    pllDetect(&d->pll);
    fpgaDetect(&d->fpga);
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

void dev_thset_read(Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t rawTemp;
        HAL_StatusTypeDef ret = adt7301_read_temp(i, &rawTemp);
        d->th[i].valid = (ret == HAL_OK);
        d->th[i].rawTemp = rawTemp;
    }
}

SensorStatus dev_thset_thermStatus(const Dev_thset *d)
{
    SensorStatus maxStatus = SENSOR_NORMAL;
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        if (!d->th[i].valid)
            continue;
        int16_t temp = adt7301_convert_temp_adt7301_scale32(d->th[i].rawTemp);
        temp /= 32;
        if (temp < tempMinCrit || temp > tempMaxCrit) {
            if (SENSOR_CRITICAL > maxStatus)
                maxStatus = SENSOR_CRITICAL;
        }
        if (temp < tempMinWarn || temp > tempMaxWarn) {
            if (SENSOR_WARNING > maxStatus)
                maxStatus = SENSOR_WARNING;
        }
    }
    return maxStatus;
}
