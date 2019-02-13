
#include "devices.h"
#include "adt7301_spi_hal.h"

void devThermInit(DevTherm d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d.rawTemp[i] = TEMP_RAW_ERROR;
    }
}

void devFpgaInit(DevFpga d)
{
    d.present = 0;
    d.id = 0;
}

void devI2cmuxInit(DevI2cmux d)
{
    d.present = 0;
}

void devInit(Devices d)
{
    devThermInit(d.therm);
    devFpgaInit(d.fpga);
    devI2cmuxInit(d.i2cmux);
}
