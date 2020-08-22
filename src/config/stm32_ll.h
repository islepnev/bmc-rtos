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

#ifdef STM32F303xC
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_spi.h"
#endif

#if defined(STM32F746xx) || defined(STM32F769xx)
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_spi.h"
#endif
