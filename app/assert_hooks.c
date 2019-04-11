//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "assert_hooks.h"
#include "debug_helpers.h"

#include "stm32f7xx_hal.h"
#include "core_cm7.h"

static void dump_extra_debug(void)
{
    debug_printf("OS ticks: %lu\n", HAL_GetTick());
    debug_printf("CPU cycles: %lu\n", DWT->CYCCNT);
}

void app_exit_handler(int result)
{
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_printf("Program exited with code %d", result);
    while(1) {
        led_blink_error();
    }
}

void vAssertCalled( uint32_t ulLine, const uint8_t *pcFile )
{
    volatile unsigned long ul = 0;

    ( void ) pcFile;
    ( void ) ulLine;

    taskDISABLE_INTERRUPTS();
    led_show_error();

    debug_printf("\nassert failed at %s:%ld\n", pcFile, ulLine);

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

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_print("Error: malloc failed\n");
    dump_extra_debug();
    while(1) {
        led_blink_error();
    }
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    (void) pxTask;
    (void) pcTaskName;
    taskDISABLE_INTERRUPTS();
    led_show_error();
    debug_print("\nError: stack overflow in task '");
    debug_print(pcTaskName);
    debug_print("'\n");
    dump_extra_debug();
    while(1) {
        led_blink_error();
    }
}
