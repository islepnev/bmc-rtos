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

#include "error_handler_impl.h"

#include <stdbool.h>

#include "led_gpio_hal.h"

static const int LED_BLINK_CPUDELAY = 200000;

//void led_blink(DeviceLeds led, int duration_on, int duration_off)
//{
//    for (int i=0; i < duration_on * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, true);
//    for (int i=0; i < duration_off * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, false);
//}

void led_show_error(void)
{
    led_set_state(LED_GREEN, false);
    led_set_state(LED_YELLOW, false);
    led_set_state(LED_RED, true);
    led_set_state(LED_INT_RED, true);
}

static void led_blink_morse(DeviceLeds led, const int buf[], unsigned int size)
{
    static unsigned int n;
    for (n=0; n<size; n++) {
        switch(buf[n]) {
        case 0:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, false);
            break;
        case 1:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, true);
            break;
        default:
            for (int i=0; i < 3 * LED_BLINK_CPUDELAY; i++)
                led_set_state(led, true);
            break;
        }
    }
}

void led_blink_error(void)
{
    // dot=1, dash=3, space=3, wordspace=7
    static const int buf[] = {
        0,0,0,0,0,0,0,   // wordspace
        1,0,1,0,1, 0,0,0, // S
        3,0,3,0,3, 0,0,0, // O
        1,0,1,0,1, 0,0,0  // S
    };
    enum { bufsize = sizeof(buf)/sizeof(buf[0])};
    led_blink_morse(LED_YELLOW, buf, bufsize);
}
