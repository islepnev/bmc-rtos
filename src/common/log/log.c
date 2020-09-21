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

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "log_prio.h"
#include "logbuffer.h"
#include "logentry.h"

osMutexId log_printf_mutex;
osMutexDef(log_printf_mutex);

void init_logging(void)
{
    log_printf_mutex = osMutexCreate(osMutex(log_printf_mutex));
    assert(log_printf_mutex);
    init_logbuffer();
}

enum {buf_size = LOG_LINE_SIZE};
static char log_printf_buffer[buf_size]; // FIXME: use BUFSIZ from stdio.h

void log_printf(LogPriority priority, const char *format, ...)
{
    static va_list args;

    osMutexWait(log_printf_mutex, osWaitForever);
    va_start(args, format);
    size_t n = vsnprintf(log_printf_buffer, sizeof log_printf_buffer, format, args);
    va_end(args);
    size_t n_written = (n > buf_size) ? buf_size : n;
    if (n_written > 0)
        log_put_long(priority, osKernelSysTick(), log_printf_buffer);
    osMutexRelease(log_printf_mutex);
}

void log_printf_debug(const char *format, ...)
{
    static va_list args;

    osMutexWait(log_printf_mutex, osWaitForever);
    va_start(args, format);
    size_t n = vsnprintf(log_printf_buffer, sizeof log_printf_buffer, format, args);
    va_end(args);
    size_t n_written = (n > buf_size) ? buf_size : n;
    if (n_written > 0)
        log_put_long(LOG_DEBUG, osKernelSysTick(), log_printf_buffer);
    osMutexRelease(log_printf_mutex);
}

void log_put(LogPriority priority, const char *str)
{
    log_put_long(priority, osKernelSysTick(), str);
}
