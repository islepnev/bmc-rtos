//
//    Copyright 2017-2019 Ilja Slepnev
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
#ifndef ADT7301_SPI_HAL_H
#define ADT7301_SPI_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal_def.h"
//#include <stdint.h>

HAL_StatusTypeDef adt7301_read_temp(int source, int16_t *data);

#ifdef __cplusplus
}
#endif

#endif // ADT7301_SPI_HAL_H
