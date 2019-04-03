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
#define LED_HEARTBEAT LED_GREEN

extern struct __I2C_HandleTypeDef * const hPll;
extern const uint8_t pllDeviceAddr;

extern struct __I2C_HandleTypeDef * const hi2c_sensors;

extern struct __SPI_HandleTypeDef * const fpga_spi;
extern struct __SPI_HandleTypeDef * const therm_spi;

void pllSetStaticPins(void);
void pllReset(void);
void pm_sensor_reset_i2c_master(void);

#endif // BSP_H
