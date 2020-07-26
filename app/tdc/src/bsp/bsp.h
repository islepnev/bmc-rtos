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

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define BOARD_TDC72 1
//#define BOARD_TDC64 1

#define LED_HEARTBEAT LED_INT_GREEN

//#define hi2c_ad9545 hi2c2
//#define hi2c_sensors hi2c4
#define hi2c_sfpiic hi2c4

#define fpga_spi hspi1
#define ad9516_spi hspi4
#define therm_spi hspi4

//#define ad9545_deviceAddr 0x4A
#define hi2c_eeprom_cfg hi2c2
//#define eeprom_cfg_deviceAddr 0x50

#define AUXPLL_AD9516_OUT6_ENABLE 1
#define AUXPLL_AD9516_OUT7_ENABLE 1
#define AUXPLL_AD9516_OUT8_ENABLE 1
#define AUXPLL_AD9516_OUT9_ENABLE 1

#ifdef BOARD_TDC64
#define DEV_THERM_COUNT 0
#endif
#ifdef BOARD_TDC72
#define DEV_THERM_COUNT 4
#endif

#ifdef BOARD_TDC72
typedef enum {
    PCB_4_1 = 0,
    PCB_4_2 = 1
} board_version_t;

// run-time detected board version (see i2c supply sensors)
extern board_version_t board_version;
#endif
typedef enum {
    PCB_VER_TDC64VHLE_V2 = 1,
} pcb_version_t;

uint32_t detect_pcb_version(void);
void update_board_version(int powermon_count);
bool fpga_done_pin_present(void);
void bsp_smbus_reset(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
