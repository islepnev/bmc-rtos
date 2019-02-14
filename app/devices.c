
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
    struct_thset_init(&d->thset);
    struct_fpga_init(&d->fpga);
    struct_pca9548_init(&d->i2cmux);
    struct_at24c_init(&d->eeprom_config);
    struct_at24c_init(&d->eeprom_vxspb);
    struct_ad9545_init(&d->pll);
    struct_powermon_init(&d->pm);
}

DeviceStatus dev_i2cmux_detect(Dev_pca9548 *d)
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
    uint8_t data = 0;
    if (HAL_OK == pca9548_read(&data))
        d->present = DEVICE_NORMAL;
    return d->present;
}

DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    uint8_t data = 0;
    if (HAL_OK == dev_eepromConfig_Read(0, &data)) {
        d->present = DEVICE_NORMAL;
    }
    return d->present;
}

DeviceStatus dev_eepromVxsPb_detect(Dev_at24c *d)
{
    uint8_t data = 0;
    if (HAL_OK == dev_eepromVxsPb_Read(0, &data)) {
        d->present = 1;
    }
    return d->present;
}

enum {
    AD9545_REG_VENDOR_ID = 0x0C,
    AD9545_REG_INT_THERM = 0x3003
};

enum {
    AD9545_VENDOR_ID = 0x0456
};

DeviceStatus pllDetect(Dev_ad9545 *d)
{
    for (int i=0; i<100; i++)
        HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    for (int i=0; i<100; i++)
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
    for (int i=0; i<100; i++)
        HAL_GPIO_ReadPin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin);

    uint32_t data = 0;
//    pllSendByte(AD9545_REG_VENDOR_ID);
//    pllReceiveByte(&data);
    pllReadRegister(AD9545_REG_VENDOR_ID, &data);
    d->present = (data == AD9545_VENDOR_ID);
//    pllReadRegister(AD9545_REG_INT_THERM, &data);
//    printf("PLL therm: %04lX\n", data);
    return d->present;
}

DeviceStatus fpgaDetect(Dev_fpga *d)
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

DeviceStatus devDetect(Devices *d)
{
    int err = 0;
    dev_i2cmux_detect(&d->i2cmux);
    dev_eepromConfig_detect(&d->eeprom_config);
    dev_eepromVxsPb_detect(&d->eeprom_vxspb);
    pllDetect(&d->pll);
    fpgaDetect(&d->fpga);
    return err == 0;
}

void dev_thset_read(Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++)
        d->th[i].rawTemp = adt7301_read_temp(i);
}

void dev_thset_print(const Dev_thset d)
{
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        print_adt7301_value(d.th[i].rawTemp);
        printf(" ");
    }
    printf("%s\n", dev_thset_thermStatus(d) ? STR_NORMAL : STR_FAIL);
}

SensorStatus dev_thset_thermStatus(const Dev_thset d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t temp = adt7301_convert_temp_adt7301_scale32(d.th[i].rawTemp);
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
