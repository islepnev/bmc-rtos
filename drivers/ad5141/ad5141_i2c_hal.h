/*
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

#ifndef AD5141_I2C_HAL_H
#define AD5141_I2C_HAL_H

#include <stdbool.h>
#include <stdint.h>

void ad5141_i2c_driver_reset(void);
bool ad5141_write(uint8_t deviceAddress, uint8_t ctrl_addr, uint8_t data);
bool ad5141_read(uint8_t deviceAddress, uint16_t command, uint8_t *data);

#endif // AD5141_I2C_HAL_H
