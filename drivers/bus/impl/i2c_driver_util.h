/*
**    Generic interrupt mode I2C driver
**
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

#ifdef __cplusplus
extern "C" {
#endif

struct __I2C_HandleTypeDef;

void i2c_driver_raise_transfer_error(struct __I2C_HandleTypeDef *hi2c);
void i2c_driver_clear_transfer_error(struct __I2C_HandleTypeDef *hi2c);
bool i2c_driver_is_transfer_ok(struct __I2C_HandleTypeDef *hi2c);

bool i2c_driver_util_init(void);
SemaphoreHandle_t i2c_driver_it_sem_by_hi2c(struct __I2C_HandleTypeDef *hi2c);
SemaphoreHandle_t i2c_driver_dev_mutex_by_index(int index);
int hi2c_index(struct __I2C_HandleTypeDef *hi2c);
struct __I2C_HandleTypeDef *hi2c_handle(BusIndex index);
int32_t i2c_driver_wait_it_sem(struct __I2C_HandleTypeDef *hi2c, uint32_t millisec);
void i2c_driver_release_it_sem(struct __I2C_HandleTypeDef *hi2c);
int32_t i2c_driver_wait_dev_mutex(int index, uint32_t millisec);
void i2c_driver_release_dev_mutex(int index);

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_UTIL_H
