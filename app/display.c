#include "display.h"
#include "adt7301_spi_hal.h"

void print_adt7301_value(uint16_t rawTemp)
{
   if (rawTemp != TEMP_RAW_ERROR) {
      int16_t temp32 = adt7301_convert_temp_adt7301_scale32(rawTemp);
      if (temp32 < 0) {
          temp32 = -temp32;
          printf("-");
      }
      else
          printf(" ");
      int16_t tempInt = temp32 / 32;
      int16_t tempDec = (temp32 - tempInt * 32) * 10 / 32;
      printf("%d.%01d", tempInt, tempDec);
   } else {
       printf(" --- ");
   }
}

void devPrintStatus(const Devices d)
{
    printf("I2C mux:       %s\n", d.i2cmux.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("EEPROM config: %s\n", d.eeprom_config.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("EEPROM VXS PB: %s\n", d.eeprom_vxspb.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("PLL I2C:       %s\n", d.pll.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("FPGA ID: %02X  %s\n", d.fpga.id, d.fpga.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
}
