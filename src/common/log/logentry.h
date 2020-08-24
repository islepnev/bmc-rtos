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

#ifndef LOGENTRY_H
#define LOGENTRY_H

#include "logbuffer.h"

#ifdef STM32F3
enum {LOG_LINE_SIZE = 100};
#else
enum {LOG_LINE_SIZE = 200};
#endif

struct LogEntry {
    LogPriority priority;
    uint32_t tick;
    char str[LOG_LINE_SIZE]; // null-terminated
};

#endif // LOGENTRY_H
