/*
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef OS_SERIAL_TTY_H
#define OS_SERIAL_TTY_H

#ifdef STM32F303xC
#include "stm32f3xx_ll_usart.h"
#endif
#if defined(STM32F746xx) || defined(STM32F769xx)
#include "stm32f7xx_ll_usart.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void initialize_serial_console_hardware(void);
void serial_console_interrupt_handler(USART_TypeDef *usart);
int __io_getchar (void);
int __io_putchar(int ch);

#ifdef __cplusplus
}
#endif

#endif // OS_SERIAL_TTY_H
