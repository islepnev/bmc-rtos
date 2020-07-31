/*
**    Generic interrupt mode SPI driver
**
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

#ifndef SPI_DRIVER_UTIL_H
#define SPI_DRIVER_UTIL_H

#include <stdbool.h>

#include "bus/bus_types.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __SPI_HandleTypeDef;

void spi_driver_raise_transfer_error(struct __SPI_HandleTypeDef *hspi);
void spi_driver_clear_transfer_error(struct __SPI_HandleTypeDef *hspi);
bool spi_driver_is_transfer_ok(struct __SPI_HandleTypeDef *hspi);

bool spi_driver_util_init(void);
SemaphoreHandle_t spi_driver_it_sem_by_hspi(struct __SPI_HandleTypeDef *hspi);
SemaphoreHandle_t spi_driver_dev_sem_by_index(int index);
int hspi_index(struct __SPI_HandleTypeDef *hspi);
struct __SPI_HandleTypeDef *hspi_handle(BusIndex index);
int32_t spi_driver_wait_it_sem(struct __SPI_HandleTypeDef *hspi, uint32_t millisec);
void spi_driver_release_it_sem(struct __SPI_HandleTypeDef *hspi);
int32_t spi_driver_wait_dev_sem(int index, uint32_t millisec);
void spi_driver_release_dev_sem(int index);

#ifdef __cplusplus
}
#endif

#endif // SPI_DRIVER_UTIL_H
