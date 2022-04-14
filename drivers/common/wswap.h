/*
**    Copyright 2022 Ilia Slepnev
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

#ifndef WSWAP_H
#define WSWAP_H

#include <stdint.h>

inline uint32_t wswap_32(uint32_t data)
{
    return ((data & 0xFFFF) << 16) | ((data >> 16) & 0xFFFF);
}

inline uint64_t wswap_64(uint64_t data)
{
    return
        ((data >> 48) & 0xFFFF) |
        (((data >> 32) & 0xFFFF) << 16) |
        (((data >> 16) & 0xFFFF) << 32) |
        ((data & 0xFFFF) << 48);
}

#endif // WSWAP_H
