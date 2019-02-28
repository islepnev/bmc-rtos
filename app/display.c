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

void devPrintStatus(const Devices *d)
{
    printf("I2C mux:       %s", d->i2cmux.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("EEPROM config: %s", d->eeprom_config.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("EEPROM VXS PB: %s", d->eeprom_vxspb.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("PLL I2C:       %s", d->pll.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("FPGA ID: %02X  %s", d->fpga.id, d->fpga.present ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    printf("%s\n", ANSI_CLEAR_EOL);
}
