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

#include "ansi_escape_codes.h"
#include "bsp_tty.h"
#include "log/log.h"
#include "stm32f7xx_ll_usart.h"

static inline void debug_send_char(const char c)
{
    // wait for UART ready
    while(! LL_USART_IsActiveFlag_TXE(TTY_USART)) {;}
    LL_USART_TransmitData8(TTY_USART, (uint8_t)c);
}

static inline void debug_print_impl(const char *ptr)
{
    enum { max_print_len = 1000 };
    for (int n = 0; n < max_print_len; n++) {
        if (*ptr == '\0')
            break;
        if (*ptr == '\n') {
            debug_send_char('\r');
        }
        debug_send_char(*ptr++);
    }
}

void debug_print(const char *str)
{
    debug_print_impl("\r" ANSI_CLEAR ANSI_CLEAR_EOL);
    debug_print_impl(str);
    debug_print_impl(ANSI_CLEAR_EOL);
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
    if (n_written > 0) {
        // debug_print(buffer);
        log_put(LOG_DEBUG, buffer);
    }
}
