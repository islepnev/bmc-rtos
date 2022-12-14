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

typedef struct fpga_spi_stats_t {
    uint32_t frames;
    uint32_t regio_timeouts;
    uint32_t regio_errors;
    uint32_t tx_crc_errors;
    uint32_t rx_crc_errors;
} fpga_spi_stats_t;

bool fpga_spi_hal_tx_rx(BusInterface *bus, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size);
bool fpga_spi_hal_tx(BusInterface *bus, uint8_t *txBuf, uint16_t Size);
bool fpga_spi_hal_rx(BusInterface *bus, uint8_t *rxBuf, uint16_t Size);
bool fpga_spi_duplex(BusInterface *bus);

void fpga_enable_interface(BusInterface *bus);
void fpga_disable_interface(BusInterface *bus);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_SPI_HAL_H */
