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

#ifndef FPGA_SPI_HAL_H
#define FPGA_SPI_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f7xx_hal.h"

enum {
    FPGA_SPI_ADDR_0 = 0,
    FPGA_SPI_ADDR_1 = 1,
    FPGA_SPI_ADDR_2 = 2,
    FPGA_SPI_ADDR_3 = 3,
    FPGA_SPI_ADDR_4 = 4,
    FPGA_SPI_ADDR_5 = 5,
    FPGA_SPI_ADDR_6 = 6,
    FPGA_SPI_ADDR_7 = 7
};

HAL_StatusTypeDef fpga_spi_hal_read_reg(uint16_t addr, uint16_t *data);
HAL_StatusTypeDef fpga_spi_hal_write_reg(uint16_t addr, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_SPI_HAL_H */
