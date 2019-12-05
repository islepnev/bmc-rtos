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

// TTVXS board specific definitions

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>

#define TTY_USART USART1 // Front panel RJ45
// #define TTY_USART USART2 // USB-RS232
#define LED_HEARTBEAT LED_INT_GREEN

#ifdef __cplusplus
extern "C" {
#endif

extern struct __I2C_HandleTypeDef * const hPll;
extern const uint8_t pllDeviceAddr;

extern struct __I2C_HandleTypeDef * const hi2c_sensors;

extern struct __SPI_HandleTypeDef * const fpga_spi;
#ifdef TTVXS_1_0
extern struct __SPI_HandleTypeDef * const therm_spi;
#else
extern struct __SPI_HandleTypeDef * const ad9516_spi;
#endif

uint32_t detect_pcb_version(void);
void pll_enable_interface(bool enable);
void pm_sensor_reset_i2c_master(void);
void fpga_enable_interface(bool enable);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
