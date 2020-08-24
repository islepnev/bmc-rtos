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

#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <stdint.h>
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F3
enum {LOG_BUF_SIZE = 20};
#else
enum {LOG_BUF_SIZE = 100};
#endif

struct LogEntry;
typedef struct LogEntry LogEntry;

void init_logbuffer(void);
void log_put_long(LogPriority priority, uint32_t tick, const char *str);
void log_get(int index, LogEntry *dest);
uint32_t log_get_wptr(void);
uint32_t log_get_count(void);

#ifdef __cplusplus
}
#endif

#endif // LOGBUFFER_H
