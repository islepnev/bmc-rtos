/*
**    Copyright 2019 Ilja Slepnev
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

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct __SPI_HandleTypeDef;

void spi_driver_init(void);

bool spi_driver_get_master_ready(struct __SPI_HandleTypeDef *hspi)
    __attribute__((warn_unused_result));

bool spi_driver_tx_rx(struct __SPI_HandleTypeDef *hspi,
                      uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool spi_driver_rx(struct __SPI_HandleTypeDef *hspi,
                   uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool spi_driver_tx(struct __SPI_HandleTypeDef *hspi,
                       uint8_t *txBuf, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

void spi_enable_interface(struct __SPI_HandleTypeDef *hspi, bool enable);

#ifdef __cplusplus
}
#endif

#endif // SPI_DRIVER_H
