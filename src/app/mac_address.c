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

#include "mac_address.h"

#include "stm32f7xx_hal.h"

void get_mac_address(uint8_t buf[6])
{
    // construct MAC address from UUID
    uint32_t uniq = (HAL_GetUIDw0() ^ HAL_GetUIDw1() ^ HAL_GetUIDw2()) & 0xFFFFFFul;

    buf[0] = 0x02;
    buf[1] = 0xA6;
    buf[2] = 0xB8;
    buf[3] = (uniq >> 16) & 0xFF;
    buf[4] = (uniq >> 8) & 0xFF;
    buf[5] = uniq & 0xFF;
}
