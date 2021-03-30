/*
**    Copyright 2021 Ilia Slepnev
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

#ifndef GPIO_UTIL_H
#define GPIO_UTIL_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32_hal.h"

#define COUNT_OF(x) (sizeof(x)/sizeof(0[x]))

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t pin;
} pin_def_t;

#ifdef __cplusplus
extern "C" {
#endif

void write_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state);
bool read_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif // GPIO_UTIL_H
