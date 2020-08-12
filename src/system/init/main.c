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

#include "app_task_init.h"
#include "app_tasks.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "hal_systick.h"
#include "init_periph.h"
#include "init_sysclk.h"
#include "led_gpio_hal.h"
#include "stm32f7xx_hal.h"
#include "tim.h"

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    led_all_set_state(true);
    init_periph();
    MX_TIM2_Init();

    app_task_init();
    create_tasks();
    led_all_set_state(false);

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    while (1) { }
}