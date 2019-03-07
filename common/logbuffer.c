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

#include "logbuffer.h"

#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"

static unsigned int log_wptr = 0;
static unsigned int log_count = 0;
static LogEntry buf[LOG_BUF_SIZE];

void log_put_long(LogPriority priority, uint32_t tick, const char *str)
{
    unsigned int copy_len = strlen(str);
    if (copy_len > LOG_LINE_SIZE-1)
        copy_len = LOG_LINE_SIZE-1;
    strncpy(buf[log_wptr].str, str, copy_len);
    buf[log_wptr].str[copy_len] = '\0';
    buf[log_wptr].priority = priority;
    buf[log_wptr].tick = tick;
    log_wptr = (log_wptr+1) % LOG_BUF_SIZE;
    log_count++;
}

void log_put(LogPriority priority, const char *str)
{
    log_put_long(priority, osKernelSysTick(), str);
}

void log_get(int index, LogEntry *dest)
{
    if (!dest)
        return;
    if (index > LOG_BUF_SIZE) {
        dest->priority = LOG_ERR;
        dest->tick = 0;
        dest->str[0] = '\0';
        return;
    }
    dest->priority = buf[index].priority;
    dest->tick = buf[index].tick;
    strncpy(dest->str, buf[index].str, LOG_LINE_SIZE);
}

uint32_t log_get_wptr(void)
{
    return log_wptr;
}

uint32_t log_get_count(void)
{
    return log_count;
}