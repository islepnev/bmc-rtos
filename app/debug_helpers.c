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

#include <stdarg.h>
#include <stdio.h>

#include "stm32f7xx_ll_usart.h"
#include "led_gpio_hal.h"
#include "bsp.h"

static void debug_send_char(const char c)
{
    // wait for UART ready
    while(! LL_USART_IsActiveFlag_TXE(TTY_USART)) {;}
    LL_USART_TransmitData8(TTY_USART, (uint8_t)c);
}

void debug_print(const char *ptr, int len)
{
    static int n;
    static const char r = '\r';
    for (n = 0; n < len; n++) {
        if (*ptr == '\n') {
            debug_send_char(r);
        }
        debug_send_char(*ptr++);
    }
}

void debug_printf(const char *format, ...)
{
    enum {buf_size = 100};
  static va_list args;
  static char buffer[buf_size]; // FIXME: use BUFSIZ from stdio.h

  va_start(args, format);
  size_t n = vsnprintf(buffer, sizeof buffer, format, args);
  va_end(args);
  size_t n_written = (n > buf_size) ? buf_size : n;
  if (n_written > 0)
      debug_print(buffer, n_written); // exclude trailing \0
}

static const int LED_BLINK_CPUDELAY = 200000;

//void led_blink(DeviceLeds led, int duration_on, int duration_off)
//{
//    for (int i=0; i < duration_on * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, LED_ON);
//    for (int i=0; i < duration_off * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, LED_OFF);
//}

void led_show_error(void)
{
    led_set_state(LED_GREEN, LED_OFF);
    led_set_state(LED_YELLOW, LED_OFF);
    led_set_state(LED_RED, LED_ON);
}

void led_blink_morse(DeviceLeds led, const int buf[], unsigned int size)
{
    static unsigned int n;
    for (n=0; n<size; n++) {
        switch(buf[n]) {
        case 0:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_OFF);
            break;
        case 1:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_ON);
            break;
        default:
            for (int i=0; i < 3 * LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_ON);
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
