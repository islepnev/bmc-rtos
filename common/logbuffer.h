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

#ifndef LOGBUFFER_C
#define LOGBUFFER_C

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_EMERG = 0,   // system is unusable
    LOG_ALERT = 1,   // action must be taken immediately
    LOG_CRIT = 2,    // critical conditions
    LOG_ERR = 3,     // error conditions
    LOG_WARNING = 4, // warning conditions
    LOG_NOTICE = 5,  // normal but significant condition
    LOG_INFO = 6,    // informational
    LOG_DEBUG = 7    // debug-level messages
} LogPriority;

enum {LOG_BUF_SIZE = 30};

struct LogEntry;
typedef struct LogEntry LogEntry;

void log_printf(LogPriority priority, const char *format, ...);
void log_printf_debug(const char *format, ...);
void log_put(LogPriority priority, const char *str);
void log_get(int index, LogEntry *dest);
uint32_t log_get_wptr(void);
uint32_t log_get_count(void);

#ifdef __cplusplus
}
#endif

#endif // LOGBUFFER_C
