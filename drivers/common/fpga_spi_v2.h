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

#ifndef FPGA_SPI_V2_H
#define FPGA_SPI_V2_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool fpga_spi_hal_read_reg(BusInterface *bus, uint16_t addr, uint16_t *data)
    __attribute__((warn_unused_result));

bool fpga_spi_hal_write_reg(BusInterface *bus, uint16_t addr, uint16_t data)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif // FPGA_SPI_V2_H
