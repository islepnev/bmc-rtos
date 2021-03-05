/*
**    VXS IIC Slave
**
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef I2C_SLAVE_DRIVER_H
#define I2C_SLAVE_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VXSIIC_BUS_ADDRESS 0x33

void iic_write_callback(uint16_t addr, uint32_t data);
void iic_read_callback(uint16_t addr, uint32_t *data);

void i2c_slave_driver_init(void);
void i2c_event_interrupt_handler(void);
void i2c_error_interrupt_handler(void);

#ifdef __cplusplus
}
#endif

#endif // I2C_SLAVE_DRIVER_H
