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

#ifndef FPGA_IO_H
#define FPGA_IO_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "devicebase.h"

enum { REGIO_WORD_SIZE = 2 };

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_fpga;

bool fpga_r16(struct Dev_fpga *d, uint32_t addr, uint16_t *data)
    __attribute__((warn_unused_result));

bool fpga_w16(struct Dev_fpga *d, uint32_t addr, uint16_t data)
    __attribute__((warn_unused_result));

bool fpga_r32(struct Dev_fpga *d, uint32_t addr, uint32_t *data)
    __attribute__((warn_unused_result));

bool fpga_w32(struct Dev_fpga *d, uint32_t addr, uint32_t data)
    __attribute__((warn_unused_result));

bool fpga_r64(struct Dev_fpga *d, uint32_t addr, uint64_t *data)
    __attribute__((warn_unused_result));

bool fpga_w64(struct Dev_fpga *d, uint32_t addr, uint64_t data)
    __attribute__((warn_unused_result));

bool fpga_read(struct Dev_fpga *d, uint32_t addr, void *buf, size_t size)
    __attribute__((warn_unused_result));

bool fpga_write(struct Dev_fpga *d, uint32_t addr, const void *buf, size_t size)
    __attribute__((warn_unused_result));

bool fpga_read_spi_status(struct Dev_fpga *d)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif // FPGA_IO_H
