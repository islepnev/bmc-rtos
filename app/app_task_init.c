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
#include "app_shared_data.h"
#include "ad9545_i2c_hal.h"
#include "FreeRTOSConfig.h"
#include "os_serial_tty.h"
#include "debug_helpers.h"
#include "dev_leds.h"

static void setStaticPins(void)
{
    pllSetStaticPins();
//    update_power_switches(&dev.pm, SWITCH_ON); // FIXME
}

static void display_led_test(void)
{
    // light all leds
    dev_led_set(&dev.leds, LED_RED,    LED_ON);
    dev_led_set(&dev.leds, LED_YELLOW, LED_ON);
    dev_led_set(&dev.leds, LED_GREEN,  LED_ON);
    // test leds
    for (int i=0; i<50000; i++) {
        dev_led_set(&dev.leds, LED_RED,    LED_ON);
        dev_led_set(&dev.leds, LED_YELLOW, LED_ON);
        dev_led_set(&dev.leds, LED_GREEN,  LED_ON);
    }
    for (int i=0; i<50000; i++) {
        dev_led_set(&dev.leds, LED_RED,    LED_OFF);
        dev_led_set(&dev.leds, LED_YELLOW, LED_OFF);
        dev_led_set(&dev.leds, LED_GREEN,  LED_OFF);
    }
    // display error state
    dev_led_set(&dev.leds, LED_RED,    LED_ON);
    dev_led_set(&dev.leds, LED_YELLOW, LED_ON);
    dev_led_set(&dev.leds, LED_GREEN,  LED_OFF);
}

void task_oneshot(void)
{
    debug_printf("\nInitializing...");

    vConfigureTimerForRunTimeStats();
    setStaticPins();
    display_led_test();
    // required for console I/O
    initialize_serial_console_hardware();
    debug_printf("Ok\n");
}
