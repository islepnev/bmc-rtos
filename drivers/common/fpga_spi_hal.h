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

#ifndef FPGA_SPI_HAL_H
#define FPGA_SPI_HAL_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    FPGA_SPI_ADDR_0 = 0,
    FPGA_SPI_ADDR_1 = 1,
    FPGA_SPI_ADDR_2 = 2,
    FPGA_SPI_ADDR_3 = 3,
    FPGA_SPI_ADDR_4 = 4,
    FPGA_SPI_ADDR_5 = 5,
    FPGA_SPI_ADDR_6 = 6,
    FPGA_SPI_ADDR_7 = 7,
    FPGA_SPI_ADDR_8 = 8,
    FPGA_SPI_ADDR_9 = 9,
    FPGA_SPI_ADDR_A = 0xA,
    FPGA_SPI_ADDR_B = 0xB,
    FPGA_SPI_ADDR_C = 0xC,
    FPGA_SPI_ADDR_D = 0xD,
    FPGA_SPI_ADDR_E = 0xE,
    FPGA_SPI_ADDR_F = 0xF
};

bool fpga_spi_hal_read_reg(BusInterface *bus, uint16_t addr, uint16_t *data);
bool fpga_spi_hal_write_reg(BusInterface *bus, uint16_t addr, uint16_t data);
void fpga_enable_interface(BusInterface *bus);
void fpga_disable_interface(BusInterface *bus);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_SPI_HAL_H */
