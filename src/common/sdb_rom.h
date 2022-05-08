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

#ifndef SDB_ROM_H
#define SDB_ROM_H

#include "sdb.h"

// 2048 bytes SDB PROM size limit
enum { SDB_MAX_RECORDS = 32 }; // 0x7C00-0x7FFF 16-bit registers capacity
enum { MAX_SDB_DEVICE_COUNT = SDB_MAX_RECORDS - 2 };

struct sdb_rom_t {
    struct sdb_interconnect ic;
    struct sdb_device device[MAX_SDB_DEVICE_COUNT];
    struct sdb_synthesis syn;
};

#endif // SDB_ROM_H
