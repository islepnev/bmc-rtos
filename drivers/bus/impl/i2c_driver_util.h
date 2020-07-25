/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef I2C_DRIVER_UTIL_H
#define I2C_DRIVER_UTIL_H

#include <stdbool.h>

#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

bool i2c_driver_util_init(void);
SemaphoreHandle_t it_sem_by_hi2c(struct __I2C_HandleTypeDef *hi2c);
SemaphoreHandle_t dev_sem_by_index(int index);
int hi2c_index(struct __I2C_HandleTypeDef *hi2c);
int32_t wait_it_sem(struct __I2C_HandleTypeDef *hi2c, uint32_t millisec);
void release_it_sem(struct __I2C_HandleTypeDef *hi2c);
int32_t wait_dev_sem(int index, uint32_t millisec);
void release_dev_sem(int index);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_UTIL_H
