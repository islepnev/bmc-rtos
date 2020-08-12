/*
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

#include "adc_driver.h"

#include <assert.h>

#include "cmsis_os.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_adc.h"
#include "debug_helpers.h"
#include "error_handler.h"
#include "log/log.h"

ADC_HandleTypeDef hadc1;

void adc_driver_init(void)
{
    // HAL_ADC_Start_IT(&hadc1);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    log_printf(LOG_DEBUG, "HAL_ADC_ErrorCallback, State %d", hadc->State);
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    uint16_t rawValue = HAL_ADC_GetValue(hadc);
//    double temp = ((float)rawValue) / 4095 * 3300;
//    temp = ((temp - 760.0) / 2.5) + 25;
//    log_printf(LOG_DEBUG, "die temp %f", temp);
//    HAL_ADC_Start_IT(hadc);
//}
