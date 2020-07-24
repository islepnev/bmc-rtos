//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef AD9545_I2C_HAL_H
#define AD9545_I2C_HAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ad9545_i2c_detect(void);
bool ad9545_read1(uint16_t reg, uint8_t *data);
bool ad9545_write1(uint16_t reg, uint8_t data);
bool ad9545_read2(uint16_t reg, uint16_t *data);
bool ad9545_write2(uint16_t reg, uint16_t data);
bool ad9545_read3(uint16_t reg, uint32_t *data);
bool ad9545_write3(uint16_t reg, uint32_t data);
bool ad9545_read4(uint16_t reg, uint32_t *data);
bool ad9545_write4(uint16_t reg, uint32_t data);
bool ad9545_read5(uint16_t reg, uint64_t *data);
bool ad9545_write5(uint16_t reg, uint64_t data);
bool ad9545_read6(uint16_t reg, uint64_t *data);
bool ad9545_write6(uint16_t reg, uint64_t data);
bool ad9545_write8(uint16_t reg, uint64_t data);

#ifdef __cplusplus
}
#endif

#endif // AD9545_I2C_HAL_H
