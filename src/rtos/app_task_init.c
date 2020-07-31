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

#include "app_task_init.h"

#include <stdlib.h>

#include "ad9545/ad9545_i2c_hal.h"
#include "FreeRTOSConfig.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "devices.h"
#include "devices_types.h"
#include "powermon/dev_powermon.h"
#include "os_serial_tty.h"
#include "ansi_escape_codes.h"
#include "debug_helpers.h"
#include "led_gpio_hal.h"
#include "bsp.h"
#include "logbuffer.h"
#include "commands.h"

static int test_cpu_tick(void)
{
    uint32_t start_ticks = DWT->CYCCNT;
    uint32_t ticks = start_ticks;
    for (int i=0; i<10; i++)
        ticks = DWT->CYCCNT;
    if (ticks == start_ticks) {
            debug_print("CPU cycle counter stopped\n");
            return -1;
    }
    debug_print("CPU cycle counter Ok\n");
    return 0;
}

static int test_hal_systick(void)
{
    const uint32_t start_ticks = HAL_GetTick();
    uint32_t tick_i1 = 0;
    uint32_t i = 0;
    const uint32_t tick_freq_hz = 1000U / uwTickFreq;
    const uint32_t cpu_cycles_per_tick = SystemCoreClock / tick_freq_hz; // loop cannot be faster than 1 cpu clock
    while(1) {
        if (i > cpu_cycles_per_tick) {
            debug_printf("HAL systick timer %d Hz stopped\n", tick_freq_hz);
            return -1;
        }
        const uint32_t tick = HAL_GetTick();
        if (tick > start_ticks && tick_i1 == 0)
            tick_i1 = i;
        if (tick > start_ticks+1) {
            debug_printf("HAL systick timer %d Hz Ok\n", tick_freq_hz);
            break;
        }
        i++;
    }
    return 0;
}

static void test_timers(void)
{
    int ret0 = test_cpu_tick();
    int ret1 = test_hal_systick();
    if (ret0 || ret1) {
        exit(1);
    }
}

void app_task_init(void)
{
    configureTimerForRunTimeStats();
    // test_timers();
    detect_pcb_version();
    initialize_serial_console_hardware();
    log_put(LOG_NOTICE, "Initializing");
//    debug_print(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR ANSI_SHOW_CURSOR "\nInitializing\n");
    // required for console I/O
//    debug_print("Waiting for threads to start\n");
    commands_init();
}
