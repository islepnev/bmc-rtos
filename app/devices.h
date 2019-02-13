#ifndef DEVICES_H
#define DEVICES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f7xx_hal.h"
enum {DEV_THERM_COUNT = 4};
typedef enum {
    SENSOR_UNKNOWN = 0,
    SENSOR_NORMAL = 1,
    SENSOR_WARNING = 2,
    SENSOR_CRITICAL = 3
} SensorStatus;

typedef struct {
//    int present;
    uint16_t rawTemp;
} Dev_adt7301;

typedef struct {
    Dev_adt7301 th[DEV_THERM_COUNT];
} Dev_th_set;

typedef struct {
    SPI_HandleTypeDef *spi;
    int present;
    uint8_t id;
} Dev_fpga;

typedef struct {
    int present;
} Dev_pca9548;

typedef struct {
    int present;
} Dev_at24c;

typedef struct {
    Dev_th_set thset;
    Dev_fpga fpga;
    Dev_pca9548 i2cmux;
    Dev_at24c eeprom_vxs_pb;
} Devices;

void dev_th_init(Dev_th_set *d);
void dev_fpga_init(Dev_fpga *d);
void dev_i2cmux_init(Dev_pca9548 *d);
void dev_eeprom_vxs_pb_init(Dev_at24c *d);
void dev_init(Devices *d);

int devDetect(Devices *d);

void dev_thset_read(Dev_th_set *d);
void dev_thset_print(const Dev_th_set d);
SensorStatus dev_thset_thermStatus(const Dev_th_set d);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
