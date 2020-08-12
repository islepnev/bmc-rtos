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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ansi_escape_codes.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "error_handler.h"
#include "log_prio.h"
#include "logentry.h"

static unsigned int log_wptr = 0;
static unsigned int log_count = 0;
osSemaphoreId buf_sem;
osSemaphoreDef(buf_sem);

static LogEntry buf[LOG_BUF_SIZE];

void init_logbuffer(void)
{
    buf_sem = osSemaphoreCreate(osSemaphore(buf_sem), 1);
    assert(buf_sem);
    debug_print("Logging initialized" ANSI_CLEAR_EOL "\n");
}

void log_put_long(LogPriority priority, uint32_t tick, const char *str)
{
    unsigned int copy_len = strlen(str);
    if (copy_len == 0)
        return;
    if (copy_len > LOG_LINE_SIZE-1)
        copy_len = LOG_LINE_SIZE-1;
    // lock
    osSemaphoreWait(buf_sem, 0);
    strncpy(buf[log_wptr].str, str, copy_len);
    if (buf[log_wptr].str[copy_len-1] == '\n')
        buf[log_wptr].str[copy_len-1] = '\0';

    buf[log_wptr].str[copy_len] = '\0';
    buf[log_wptr].priority = priority;
    buf[log_wptr].tick = tick;
    log_wptr = (log_wptr+1) % LOG_BUF_SIZE;
    // unlock
    osSemaphoreRelease(buf_sem);
    log_count++;
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
    osSemaphoreWait(buf_sem, 0);
    dest->priority = buf[index].priority;
    dest->tick = buf[index].tick;
    strncpy(dest->str, buf[index].str, LOG_LINE_SIZE);
    osSemaphoreRelease(buf_sem);
}

uint32_t log_get_wptr(void)
{
    return log_wptr;
}

uint32_t log_get_count(void)
{
    return log_count;
}
