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

#include "error_handler.h"

#include "stm32f7xx_hal_conf.h"
#include "cmsis_os.h"
//#include "core_cm7.h"

#include "debug_helpers.h"
#include "error_handler_impl.h"

void app_exit_handler(int result)
{
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_printf("\nProgram exited with code %d\n", result);
    while(1) {
        led_blink_error();
    }
}

static void app_assert_handler( uint32_t line, const uint8_t *file )
{
    volatile unsigned long ul = 0;

    taskDISABLE_INTERRUPTS();
    led_show_error();

    debug_printf("\nassert failed at %s:%ld\n", file, line);

    taskENTER_CRITICAL();
    {
        /* Set ul to a non-zero value using the debugger to step out of this
        function. */
        while( ul == 0 )
        {
            led_blink_error();
//            __NOP();
        }
    }
    taskEXIT_CRITICAL();
}

/**
 * @brief STM32 HAL error handler
 */
void Error_Handler(void)
{
    led_show_error();
    while(1)
    {
        led_blink_error();
    }
}

/**
  * @brief used in STM32 HAL by assert_param when USE_FULL_ASSERT is defined
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    app_assert_handler(line, file);
}

/**
 * @brief used in FreeRTOS when configUSE_MALLOC_FAILED_HOOK is defined
 */
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_print("\nError: malloc failed\n");
    while(1) {
        led_blink_error();
    }
}

/**
 * @brief used in FreeRTOS when configCHECK_FOR_STACK_OVERFLOW is defined
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    (void) pxTask;
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_print("\nError: stack overflow in task '");
    debug_print(pcTaskName);
    debug_print("'\n");
    while(1) {
        led_blink_error();
    }
}
