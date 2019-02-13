
#include "devices.h"
#include "adt7301_spi_hal.h"
#include "pca9548_i2c_hal.h"
#include "i2c.h"
#include "display.h"

void dev_th_init(Dev_th_set *d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].rawTemp = TEMP_RAW_ERROR;
    }
}

void dev_fpga_init(Dev_fpga *d)
{
    d->present = 0;
    d->id = 0;
}

void dev_i2cmux_init(Dev_pca9548 *d)
{
    d->present = 0;
}

void dev_init(Devices *d)
{
    dev_th_init(&d->thset);
    dev_fpga_init(&d->fpga);
    dev_i2cmux_init(&d->i2cmux);
}

int dev_i2cmux_detect(Dev_pca9548 *d)
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
    uint8_t data = 0;
    if (HAL_OK == pca9548_read(&data))
        d->present = 1;
    return d->present;
}

HAL_StatusTypeDef eepromVxsPbRead(uint16_t addr, uint8_t *data)
{
    const int eepromVxsPbDeviceAddr = 0x51;
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c1, eepromVxsPbDeviceAddr << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, 100);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

int eepromVxsPbDetect()
{
    uint8_t data = 0;
    printf("EEPROM VXS PB: ");
    if (HAL_OK == eepromVxsPbRead(0, &data)) {
        printf("%02X %s\n", data, STR_NORMAL);
    } else {
        printf("%s\n", STR_FAIL);
    }
    return 1;
}

int devDetect(Devices *d)
{
    int err = 0;
    dev_i2cmux_detect(&d->i2cmux);
    return err == 0;
}

void dev_thset_read(Dev_th_set *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++)
        d->th[i].rawTemp = adt7301_read_temp(i);
}

void dev_thset_print(const Dev_th_set d)
{
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        print_adt7301_value(d.th[i].rawTemp);
        printf(" ");
    }
    printf("%s\n", dev_thset_thermStatus(d) ? STR_NORMAL : STR_FAIL);
}

SensorStatus dev_thset_thermStatus(const Dev_th_set d)
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
