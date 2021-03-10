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

#include "rtc_util.h"

#include <time.h>
#include <sys/time.h>

#include "rtc.h"

#ifdef STM32F303xC
#include "stm32f3xx_hal.h"
#endif

#if defined(STM32F746xx) || defined(STM32F769xx)
#include "stm32f7xx_hal.h"
#endif

bool get_rtc_tm_us(struct tm *tm, uint32_t *microsec)
{
    if (!tm) return false;
    if (hrtc.State != HAL_RTC_STATE_READY)
        return false;
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_StatusTypeDef ret1 = HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    // Call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values
    // in the higher-order calendar shadow registers
    HAL_StatusTypeDef ret2 = HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (HAL_OK != ret1 || HAL_OK != ret2)
        return false;
    tm->tm_hour = sTime.Hours;
    tm->tm_min  = sTime.Minutes;
    tm->tm_sec  = sTime.Seconds;
    tm->tm_wday = sDate.WeekDay % 7;
    tm->tm_mon  = sDate.Month - 1;
    tm->tm_mday = sDate.Date;
    tm->tm_year = 100 + sDate.Year;
    tm->tm_isdst = sTime.DayLightSaving;
    tm->tm_yday = 0; // not set
    if (microsec) {
        *microsec = (int64_t)(sTime.SecondFraction - sTime.SubSeconds) * 1000000ULL / (sTime.SecondFraction + 1);
    }
    return true;
}

bool get_rtc_tm(struct tm *tm)
{
    uint32_t us;
    return get_rtc_tm_us(tm, &us);
}

bool set_rtc_tm(const struct tm *tm, uint32_t microsec)
{
    if (hrtc.State != HAL_RTC_STATE_READY)
        return false;
    RTC_TimeTypeDef sTime = {0};
    sTime.Hours   = tm->tm_hour;
    sTime.Minutes = tm->tm_min;
    sTime.Seconds = tm->tm_sec;
    sTime.SecondFraction = (uint32_t)(hrtc.Init.SynchPrediv+1) * microsec / 1000000; // not implemented by HAL
    sTime.DayLightSaving = tm->tm_isdst ? RTC_DAYLIGHTSAVING_NONE : RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    bool ret1 = (HAL_OK == HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN));

    RTC_DateTypeDef sDate = {0};
    sDate.WeekDay = (tm->tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : tm->tm_wday;
    sDate.Month = RTC_MONTH_JANUARY + tm->tm_mon;
    sDate.Date = tm->tm_mday;
    sDate.Year = tm->tm_year - 100;
    bool ret2 = (HAL_OK == HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN));
    return ret1 && ret2;
}

bool get_rtc_tv(struct timeval *tv)
{
    struct tm tm;
    uint32_t us;
    if (!get_rtc_tm_us(&tm, &us))
        return false;
    time_t t = mktime(&tm);
    if ((time_t)(-1) == t)
        return false;
    if (!tv)
        return true;
    tv->tv_sec = t;
    tv->tv_usec = us;
    return true;
}

bool set_rtc_tv(const struct timeval *tv)
{
    struct tm tm;
    const time_t t = tv->tv_sec;
    if (0 == localtime_r(&t, &tm))
        return false;
    bool ok = set_rtc_tm(&tm, tv->tv_usec);
    return ok;
}
