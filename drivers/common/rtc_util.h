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
#ifndef RTC_UTIL_H
#define RTC_UTIL_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tm;
struct timeval;

bool get_rtc_tm(struct tm *tm);
bool get_rtc_tm_us(struct tm *tm, uint32_t *microsec);
bool set_rtc_tm(const struct tm *tm, uint32_t microsec);

bool get_rtc_tv(struct timeval *tv);
bool set_rtc_tv(const struct timeval *tv);

#ifdef __cplusplus
}
#endif

#endif // RTC_UTIL_H
