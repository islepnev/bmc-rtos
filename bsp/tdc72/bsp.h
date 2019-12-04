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

// TDC72VHL v4 board specific definitions

#ifndef BSP_H
#define BSP_H

#include <stdint.h>

#define TTY_USART USART3
#define LED_HEARTBEAT LED_INT_GREEN

extern struct __SPI_HandleTypeDef * const fpga_spi;
extern struct __SPI_HandleTypeDef * const therm_spi;
extern struct __I2C_HandleTypeDef * const hPll;
extern struct __I2C_HandleTypeDef * const hi2c_sensors;

typedef enum {PLL_ENABLE = 0, PLL_DISABLE = 1} pll_enable_t;
void pllSetStaticPins(pll_enable_t enable);
void pllReset(void);

typedef enum {PLL_GPIO_TEST_FAIL = 0, PLL_GPIO_TEST_OK = 1} pll_gpio_test_t;
pll_gpio_test_t pll_gpio_test(void);

typedef enum {
    PCB_4_1 = 0,
    PCB_4_2 = 1
} board_version_t;

// run-time detected board version (see i2c supply sensors)
extern board_version_t board_version;

void update_board_version(int powermon_count, int pots_count);

#endif // BSP_H
