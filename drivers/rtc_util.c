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

#include "rtc.h"

#ifdef STM32F303xC
#include "stm32f3xx_hal.h"
#endif

#if defined(STM32F746xx) || defined(STM32F769xx)
#include "stm32f7xx_hal.h"
#endif

void get_rtc_tm(struct tm *tm)
{
    if (!tm) return;
    if (hrtc.State != HAL_RTC_STATE_READY)
        return;
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_StatusTypeDef ret1 = HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_StatusTypeDef ret2 = HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // call HAL_RTC_GetDate() after HAL_RTC_GetTime
    if (HAL_OK != ret1 || HAL_OK != ret2)
        return;
    tm->tm_hour = sTime.Hours;
    tm->tm_min  = sTime.Minutes;
    tm->tm_sec  = sTime.Seconds;
    tm->tm_wday = sDate.WeekDay % 7;
    tm->tm_mon  = sDate.Month - 1;
    tm->tm_mday = sDate.Date;
    tm->tm_year = 100 + sDate.Year;
}
