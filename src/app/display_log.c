/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "display_log.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "log/logbuffer.h"
#include "log/logentry.h"
#include "devicelist_print.h"
#include "display.h"

static void print_log_entry(uint32_t index)
{
    struct LogEntry ent;
    log_get(index, &ent);
    const char *prefix = "";
    const char *suffix = ANSI_CLEAR_EOL ANSI_CLEAR;
    switch (ent.priority) {
    case LOG_EMERG: prefix = ANSI_PUR; break;
    case LOG_ALERT: prefix = ANSI_PUR; break;
    case LOG_CRIT: prefix = ANSI_PUR; break;
    case LOG_ERR: prefix = ANSI_RED; break;
    case LOG_WARNING: prefix = ANSI_YELLOW; break;
    case LOG_NOTICE: break;
    case LOG_INFO: break;
    case LOG_DEBUG: break;
    default: prefix = ANSI_PUR; break;
    }
    printf("%s%8ld.%03ld %s%s", prefix,
           ent.tick/1000, ent.tick%1000, ent.str, suffix);
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_log_lines(int count)
{
    uint32_t max_count = count;
    if (max_count > LOG_BUF_SIZE)
        max_count = LOG_BUF_SIZE;
    volatile const uint32_t log_count = log_get_count();
    volatile const uint32_t log_wptr = log_get_wptr();
    volatile const uint32_t log_start = (log_count > max_count) ? (log_wptr + LOG_BUF_SIZE - max_count) % LOG_BUF_SIZE : 0;
    if (log_start <= log_wptr) {
        for (uint32_t i=log_start; i<log_wptr; i++)
            print_log_entry(i);
    } else {
        for (uint32_t i=log_start; i<LOG_BUF_SIZE; i++)
            print_log_entry(i);
        for (uint32_t i=0; i<log_wptr; i++)
            print_log_entry(i);
    }
    for (uint32_t i=log_count; i<max_count; i++)
        print_clear_eol();
}

void print_log_messages(int y, int count)
{
    //    print_clearbox(y, DISPLAY_LOG_H);
    print_goto(y, 1);
    if (count > 0)
        print_log_lines(count);
}

void display_log(int y, int count)
{
    print_goto(2, 1);
    printf("Log messages\n" ANSI_CLEAR_EOL);
    print_goto(y, 1);
    print_log_lines(count);
}

