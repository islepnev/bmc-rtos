/*
**    Copyright 2020 Ilia Slepnev
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

#ifndef SDB_CRC16_H
#define SDB_CRC16_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint16_t crc16_poly;
extern const uint16_t crc16_init;

uint16_t sdb_crc16_d8(uint16_t init, const uint8_t buf[], size_t len);
uint16_t sdb_crc16_be16(uint16_t init, const uint16_t buf[], size_t len);

#ifdef __cplusplus
}
#endif

#endif // SDB_CRC16_H
