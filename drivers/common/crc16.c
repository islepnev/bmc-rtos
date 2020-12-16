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

#include "crc16.h"

const uint16_t crc16_poly = 0x8BB7; // CRC-16-T10-DIF (SCSI DIF)
const uint16_t crc16_init = 0xFFFF; // differs from standard init 0

uint16_t crc16_d8(uint8_t buf[], size_t len)
{
    uint16_t crc = crc16_init;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)(buf[i]) << 8;
        for (int k = 0; k < 8; k++)
            crc = crc & 0x8000 ? (crc << 1) ^ crc16_poly : crc << 1;
    }
    return crc;
}

uint16_t crc16_be16(uint16_t buf[], size_t len)
{
    uint16_t crc = crc16_init;
    for (size_t i = 0; i < len; i++) {
        // MSB
        uint16_t data = (buf[i] >> 8) & 0xFF;
        crc ^= (uint16_t)(data) << 8;
        for (int k = 0; k < 8; k++)
            crc = crc & 0x8000 ? (crc << 1) ^ crc16_poly : crc << 1;
        // LSB
        data = (buf[i]) & 0xFF;
        crc ^= (uint16_t)(data) << 8;
        for (int k = 0; k < 8; k++)
            crc = crc & 0x8000 ? (crc << 1) ^ crc16_poly : crc << 1;
    }
    return crc;
}
