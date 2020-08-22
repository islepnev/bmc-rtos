/*
**    TDC72VHL SPI Init/Deinit
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

#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

struct __SPI_HandleTypeDef;

extern struct __SPI_HandleTypeDef hspi1;
extern struct __SPI_HandleTypeDef hspi2;
extern struct __SPI_HandleTypeDef hspi3;
extern struct __SPI_HandleTypeDef hspi4;
extern struct __SPI_HandleTypeDef hspi5;

void init_spi_peripherals(void);

#ifdef __cplusplus
}
#endif

#endif // SPI_H
