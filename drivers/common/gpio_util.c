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

#include "gpio_util.h"

#include <stddef.h>

void write_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state)
{
    const GPIO_PinState write = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(gpio, pin, write);
}

bool read_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin)
{
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET;
}
