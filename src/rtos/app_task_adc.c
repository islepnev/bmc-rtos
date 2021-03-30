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

#include "app_task_adc.h"

#include <assert.h>

#include "adc_driver.h"
#include "app_shared_data.h"
#include "app_tasks.h"
#include "cmsis_os.h"
#include "log/log.h"
#include "stm32_hal.h"

extern ADC_HandleTypeDef hadc1;

static const uint32_t adcTaskLoopDelay = 1000;
osThreadId adcThreadId = NULL;
enum { adcThreadStackSize = threadStackSize };

static void start_adc_task(void const *arg)
{
    (void) arg;
    adc_driver_init();
    HAL_ADC_Start(&hadc1);

    while(1) {
        if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, 0)) {
            uint16_t rawValue = HAL_ADC_GetValue(&hadc1);
            double temp = ((double)rawValue) / 4095 * 3300;
            temp = ((temp - 760.0) / 2.5) + 25;
            // log_printf(LOG_DEBUG, "CPU temp %.2f", temp);
        }

        HAL_ADC_Start(&hadc1);
        osDelay(adcTaskLoopDelay);
    }
}

osThreadDef(adc, start_adc_task, osPriorityLow,      1, adcThreadStackSize);

void create_task_adc(void)
{
    adcThreadId = osThreadCreate(osThread (adc), NULL);
    assert(adcThreadId);
}
