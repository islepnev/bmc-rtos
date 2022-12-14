/*
**    Copyright 2021 Ilia Slepnev
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

#ifndef SDB_UTIL_H
#define SDB_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct sdb_component;
struct sdb_interconnect;
struct sdb_synthesis;
struct sdb_device;
struct sdb_rom_t;

void sdb_component_fix_endian(struct sdb_component *p);
void sdb_interconnect_fix_endian(struct sdb_interconnect *p);
void sdb_synthesis_fix_endian(struct sdb_synthesis *p);
void sdb_device_fix_endian(struct sdb_device *p);
bool sdb_ic_validate(const struct sdb_interconnect *p);
bool sdb_dev_validate(const struct sdb_device *p);
bool sdb_checksum_present(const struct sdb_rom_t *sdb);
bool sdb_validate_checksum_nr(const struct sdb_rom_t *sdb);
bool sdb_validate_checksum(const struct sdb_rom_t *sdb);
uint16_t sdb_checksum(const struct sdb_rom_t *sdb);
void sdb_fill_checksum(struct sdb_rom_t *sdb);
void sdb_copy_printable(char *dest, const uint8_t *buf, size_t size, char fill);
void fill_sdb_string(uint8_t *buf, uint8_t size, const char *str);
int snprint_sdb_version(char *str, size_t size, uint32_t version);
int snprint_sdb_date(char *str, size_t size, uint32_t date);

#ifdef __cplusplus
}
#endif

#endif // SDB_UTIL_H
