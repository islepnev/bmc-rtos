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

#include "app_task_init.h"
#include <stdlib.h>
#include "app_shared_data.h"
#include "ad9545_i2c_hal.h"
#include "FreeRTOSConfig.h"
#include "dev_powermon.h"
#include "os_serial_tty.h"
#include "ansi_escape_codes.h"
#include "debug_helpers.h"
#include "led_gpio_hal.h"
#include "cmsis_os.h"

static void setStaticPins(void)
{
    pllSetStaticPins();
    update_power_switches(&dev.pm, SWITCH_ON); // FIXME
}

static int test_cpu_tick(void)
{
    uint32_t start_ticks = DWT->CYCCNT;
    uint32_t ticks = start_ticks;
    for (int i=0; i<10; i++)
        ticks = DWT->CYCCNT;
    if (ticks == start_ticks) {
            debug_printf("CPU cycle counter stopped\n");
            return -1;
    }
    debug_printf("CPU cycle counter Ok\n");
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
    led_all_set_state(LED_ON);
    debug_printf(ANSI_CLEAR ANSI_SHOW_CURSOR "\nInitializing\n");
    configureTimerForRunTimeStats();
    setStaticPins();
    test_timers();
    // required for console I/O
    initialize_serial_console_hardware();
    debug_printf("Waiting for threads to start\n");
    led_all_set_state(LED_OFF);
}
