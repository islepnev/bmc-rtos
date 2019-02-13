#ifndef DEVICES_H
#define DEVICES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

enum {DEV_THERM_COUNT = 4};

typedef struct {
//    int present[DEV_THERM_COUNT];
    uint16_t rawTemp[DEV_THERM_COUNT];
}  DevTherm;

typedef struct {
    int present;
    uint8_t id;
} DevFpga;

typedef struct {
    int present;
} DevI2cmux;

typedef struct {
    DevTherm therm;
    DevFpga fpga;
    DevI2cmux i2cmux;
} Devices;

void devThermInit(DevTherm d);
void devFpgaInit(DevFpga d);
void devI2cmuxInit(DevI2cmux d);
void devInit(Devices d);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
