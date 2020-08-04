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

#include "spi_driver_callbacks.h"

#include "spi.h"
#include "spi_driver_util.h"

void HAL_SPI_TxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_it_sem(hspi);
}

void HAL_SPI_RxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_it_sem(hspi);
}

void HAL_SPI_TxRxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_it_sem(hspi);
}

void HAL_SPI_ErrorCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_raise_transfer_error(hspi);
    spi_driver_release_it_sem(hspi);

//    if (!hspi)
//        Error_Handler();
//    log_printf(LOG_WARNING, "%s SPI%d error, code %d\n", __func__, hspi_index(hspi), hspi->ErrorCode);
//    // reinitialize SPI
//    spi_driver_reset(hspi);
//    spi_driver_release_it_sem(hspi);
}

void HAL_SPI_AbortCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_it_sem(hspi);
}
