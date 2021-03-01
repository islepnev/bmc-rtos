/*
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

#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "stm32f7xx_hal_gpio.h"

void MX_GPIO_Init(void);

void write_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state);
bool read_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
