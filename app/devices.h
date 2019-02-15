#ifndef DEVICES_H
#define DEVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "dev_types.h"
#include "dev_powermon.h"
#include "dev_leds.h"

enum { FPGA_DEVICE_ID = 0x68}; // FIXME: 0xD0

typedef struct {
    Dev_Leds leds;
    Dev_thset thset;
    Dev_fpga fpga;
    Dev_pca9548 i2cmux;
    Dev_at24c eeprom_vxspb;
    Dev_at24c eeprom_config;
    Dev_ad9545 pll;
    Dev_powermon pm;
} Devices;

void struct_thset_init(Dev_thset *d);
void struct_fpga_init(Dev_fpga *d);
void struct_pca9548_init(Dev_pca9548 *d);
void struct_at24c_init(Dev_at24c *d);
void struct_ad9545_init(Dev_ad9545 *d);
void struct_Devices_init(Devices *d);

DeviceStatus devDetect(Devices *d);
void dev_switchPower(Devices *dev, SwitchOnOff state);
void dev_read_thermometers(Devices *dev);
void dev_print_thermometers(const Devices dev);

//void dev_thset_read(Dev_thset *d);
//void dev_thset_print(const Dev_thset d);
//SensorStatus dev_thset_thermStatus(const Dev_thset d);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
