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
#include "pca9548_i2c_hal.h"
#include "ad9545_i2c_hal.h"
#include "fpga_spi_hal.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "i2c.h"
#include "display.h"

void struct_thset_init(Dev_thset *d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].rawTemp = TEMP_RAW_ERROR;
    }
}

void struct_fpga_init(Dev_fpga *d)
{
    d->present = DEVICE_UNKNOWN;
    d->id = 0;
}

void struct_pca9548_init(Dev_pca9548 *d)
{
    d->present = DEVICE_UNKNOWN;
}

void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

void struct_ad9545_init(Dev_ad9545 *d)
{
    d->present = DEVICE_UNKNOWN;
}

void struct_Devices_init(Devices *d)
{
    struct_dev_leds_init(&d->leds);
    struct_thset_init(&d->thset);
    struct_fpga_init(&d->fpga);
    struct_pca9548_init(&d->i2cmux);
    struct_at24c_init(&d->eeprom_config);
    struct_at24c_init(&d->eeprom_vxspb);
    struct_ad9545_init(&d->pll);
}

static DeviceStatus dev_i2cmux_detect(Dev_pca9548 *d)
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
    uint8_t data = 0;
    if (HAL_OK == pca9548_read(&data))
        d->present = DEVICE_NORMAL;
    return d->present;
}

static DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    d->present = (HAL_OK == dev_eepromConfig_Detect());
//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromConfig_Read(0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

static DeviceStatus dev_eepromVxsPb_detect(Dev_at24c *d)
{
    d->present = (HAL_OK == dev_eepromVxsPb_Detect());
//    uint8_t data = 0;
//    if (HAL_OK == dev_eepromVxsPb_Read(0, &data)) {
//        d->present = 1;
//    }
    return d->present;
}

enum {
    AD9545_REG_VENDOR_ID = 0x0C,
    AD9545_REG_INT_THERM = 0x3003
};

enum {
    AD9545_VENDOR_ID = 0x0456
};

static void pllReset(Dev_ad9545 *d)
{
    for (int i=0; i<100; i++)
        HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    for (int i=0; i<100; i++)
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
    for (int i=0; i<100; i++)
        HAL_GPIO_ReadPin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin);
}

static DeviceStatus pllDetect(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = ad9545_detect();
    d->present = (HAL_OK == ret);
    if (d->present) {
        uint32_t data = 0;
        ad9545_read(AD9545_REG_VENDOR_ID, &data);
        d->present = (data == AD9545_VENDOR_ID);
    }
    /*
//    pllSendByte(AD9545_REG_VENDOR_ID);
//    pllReceiveByte(&data);
    pllReadRegister(AD9545_REG_VENDOR_ID, &data);
    d->present = (data == AD9545_VENDOR_ID);
//    pllReadRegister(AD9545_REG_INT_THERM, &data);
//    printf("PLL therm: %04lX\n", data);
*/
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
    d->present = (err == 0);
    d->id = id;
    return d->present;
}
DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if ((d->i2cmux.present == DEVICE_NORMAL)
//            && (d->eeprom_config.present == DEVICE_NORMAL)
            && (d->eeprom_vxspb.present == DEVICE_NORMAL)
            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

void devReset(Devices *d)
{
    pllReset(&d->pll);
}

DeviceStatus devDetect(Devices *d)
{
    dev_i2cmux_detect(&d->i2cmux);
    dev_eepromConfig_detect(&d->eeprom_config);
    dev_eepromVxsPb_detect(&d->eeprom_vxspb);
    pllDetect(&d->pll);
    fpgaDetect(&d->fpga);
    return getDeviceStatus(d);
}

void dev_switchPower(Dev_powermon *pm, SwitchOnOff state)
{
    update_power_switches(pm, state);
}

PgoodState dev_readPgood(Dev_powermon *pm)
{
    pm_read_pgood(pm);
    return (pm->fpga_core_pgood && pm->ltm_pgood) ? PGOOD_OK : PGOOD_FAIL;
}

void dev_waitPgood(Devices *dev, SwitchOnOff state)
{
    update_power_switches(&dev->pm, state);
    pm_read_pgood(&dev->pm);
    if (state == SWITCH_ON) {
        // Wait for PGOOD
        const uint32_t PGOOD_TIMEOUT_MS = 100;
        uint32_t tickStart = HAL_GetTick();
        while (1) {
            int pgood = dev_readPgood(&dev->pm);
            if (pgood) {
                break;
            }
            if ((HAL_GetTick() - tickStart) > PGOOD_TIMEOUT_MS) {
                //             printf("No power %s\n", STR_FAIL);
                break;
            }
        }
    }
//    pm_read_pgood(&dev->pm);
//    pm_pgood_print(dev->pm);
}

static void dev_thset_read(Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++)
        d->th[i].rawTemp = adt7301_read_temp(i);
}

static SensorStatus dev_thset_thermStatus(const Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t temp = adt7301_convert_temp_adt7301_scale32(d->th[i].rawTemp);
        temp /= 32;
        const int tempMinCrit = -40;
        const int tempMaxCrit = 80.0;
        if (temp < tempMinCrit || temp > tempMaxCrit)
            return SENSOR_CRITICAL;
        const int tempMinWarn = 0.1;
        const int tempMaxWarn = 60.0;
        if (temp < tempMinWarn || temp > tempMaxWarn)
            return SENSOR_WARNING;
    }
    return SENSOR_NORMAL;
}

static void dev_thset_print(const Dev_thset *d)
{
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        print_adt7301_value(d->th[i].rawTemp);
        printf(" ");
    }
    printf("%s\n", dev_thset_thermStatus(d) ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
}

void dev_read_thermometers(Devices *dev)
{
    if (pm_sensor_isValid(dev->pm.sensors[SENSOR_VME_5V])) { // 5V
        for (int i=0; i<DEV_THERM_COUNT; i++)
            dev_thset_read(&dev->thset);
    }
}

void dev_print_thermometers(const Devices *dev)
{
    if (pm_sensor_isValid(dev->pm.sensors[SENSOR_VME_5V])) { // 5V
        dev_thset_print(&dev->thset);
    } else {
        printf("Temp: no power\n");
    }
}
