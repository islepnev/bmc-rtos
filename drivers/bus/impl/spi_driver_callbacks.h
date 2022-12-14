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

#ifndef SPI_DRIVER_CALLBACKS_H
#define SPI_DRIVER_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

struct __SPI_HandleTypeDef;
void HAL_SPI_TxCpltCallback(struct __SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(struct __SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(struct __SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(struct __SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif // SPI_DRIVER_CALLBACKS_H
