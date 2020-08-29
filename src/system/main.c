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
#include "debug_helpers.h"
#include "gpio.h"
#include "hal_systick.h"
#include "stm32_init_periph.h"
#include "init_periph.h"
#include "init_sysclk.h"
#include "led_gpio_hal.h"
#include "stm32_hal.h"

int main(void)
{
#ifdef STM32F7
    SCB_EnableICache();
    SCB_EnableDCache();
#endif
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    gpio_enable_clock();
    MX_GPIO_Init();
    led_all_set_state(true);
    init_periph();

    app_task_init();
    create_tasks();
    led_all_set_state(false);
    debug_printf("Starting kernel\n");

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    while (1) { }
}
