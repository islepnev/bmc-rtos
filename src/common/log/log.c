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

#include "log.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "log_prio.h"
#include "logbuffer.h"
#include "logentry.h"

void log_printf(LogPriority priority, const char *format, ...)
{
    enum {buf_size = LOG_LINE_SIZE};
    static va_list args;
    static char buffer[buf_size]; // FIXME: use BUFSIZ from stdio.h

    va_start(args, format);
    size_t n = vsnprintf(buffer, sizeof buffer, format, args);
    va_end(args);
    size_t n_written = (n > buf_size) ? buf_size : n;
    if (n_written > 0)
        log_put_long(priority, osKernelSysTick(), buffer);
}

void log_printf_debug(const char *format, ...)
{
    enum {buf_size = LOG_LINE_SIZE};
    static va_list args;
    static char buffer[buf_size]; // FIXME: use BUFSIZ from stdio.h

    va_start(args, format);
    size_t n = vsnprintf(buffer, sizeof buffer, format, args);
    va_end(args);
    size_t n_written = (n > buf_size) ? buf_size : n;
    if (n_written > 0)
        log_put_long(LOG_DEBUG, osKernelSysTick(), buffer);
}

void log_put(LogPriority priority, const char *str)
{
    log_put_long(priority, osKernelSysTick(), str);
}
