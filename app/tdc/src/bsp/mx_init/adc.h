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

#ifndef ADC_H
#define ADC_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_adc.h"

bool MX_ADC1_Init(void);

extern ADC_HandleTypeDef hadc1;

#ifdef __cplusplus
}
#endif

#endif // ADC_H
