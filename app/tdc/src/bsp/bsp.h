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

#if !defined(BOARD_TDC64) && \
!defined(BOARD_TDC72) && \
!defined(BOARD_TDC72VHLV2) && \
!defined(BOARD_TDC72VHLV3)
#error
#endif

#if defined(BOARD_TDC64)
#define ENABLE_AD9516    1
#define ENABLE_AD9545    1
#define ENABLE_AD9548    0
#define ENABLE_ADT7301   1
#define ENABLE_DIGIPOT   1
#define ENABLE_MAX31725  1
#define ENABLE_POWERMON  1
#define ENABLE_SFPIIC    1
#define ENABLE_SENSORS   1
#endif

#if defined(BOARD_TDC72)
#define ENABLE_AD9516    0
#define ENABLE_AD9545    1
#define ENABLE_AD9548    0
#define ENABLE_ADT7301   1
#define ENABLE_DIGIPOT   1
#define ENABLE_MAX31725  1
#define ENABLE_POWERMON  1
#define ENABLE_SFPIIC    1
#define ENABLE_SENSORS   1
#endif

#if defined(BOARD_TDC72VHLV2)
#define ENABLE_AD9516    0
#define ENABLE_AD9548    1
#define ENABLE_ADT7301   1
#define ENABLE_DIGIPOT   0
#define ENABLE_MAX31725  0
#define ENABLE_POWERMON  0
#define ENABLE_SFPIIC    0
#define ENABLE_SENSORS   0
#endif

#if defined(BOARD_TDC72VHLV3)
#define ENABLE_AD9516    0
#define ENABLE_AD9548    1
#define ENABLE_ADT7301   1
#define ENABLE_DIGIPOT   0
#define ENABLE_MAX31725  0
#define ENABLE_POWERMON  0
#define ENABLE_SFPIIC    0
#define ENABLE_SENSORS   0
#endif

#ifdef BOARD_TDC72VHLV3
#define USART3_SWAP_RXTX 1
#define LED_HEARTBEAT LED_GREEN
#else
#define USART3_SWAP_RXTX 0
#define LED_HEARTBEAT LED_INT_GREEN
#endif

#define FPGA_SPI_BUS_INDEX 1
#define AD9516_SPI_BUS_INDEX 4
#define AD9548_SPI_BUS_INDEX 5

#define AUXPLL_AD9516_OUT6_ENABLE 1
#define AUXPLL_AD9516_OUT7_ENABLE 1
#define AUXPLL_AD9516_OUT8_ENABLE 1
#define AUXPLL_AD9516_OUT9_ENABLE 1

#define TDC64_MAX31725_COUNT 2
#define TDC72_ADT7301_COUNT 4

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

extern int pcb_version;
uint32_t read_pcb_version(void)  __attribute__((warn_unused_result));
void update_board_version(int powermon_count);
bool fpga_done_pin_present(void);
void sfpiic_switch_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
