/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#include "sntp/sntp_client.h"

#include <time.h>

#include "lwip/opt.h"
#include "lwip/apps/sntp.h"
#include "lwip/netif.h"
#include "stm32f7xx_hal_rtc.h"
#include "rtc.h"

void
sntp_client_set_system_time(uint32_t sec, uint32_t microsec)
{
  char buf[32];
  struct tm current_time_val;
  time_t current_time = (time_t)sec;

#ifdef _MSC_VER
  localtime_s(&current_time_val, &current_time);
#else
  localtime_r(&current_time, &current_time_val);
#endif

  strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
  LWIP_PLATFORM_DIAG(("SNTP time: %s\n", buf));
  RTC_TimeTypeDef sTime = {0};
  sTime.Hours   = current_time_val.tm_hour;
  sTime.Minutes = current_time_val.tm_min;
  sTime.Seconds = current_time_val.tm_sec;
  sTime.SecondFraction = (u32_t)(hrtc.Init.SynchPrediv+1) * microsec / 1000000;
  sTime.DayLightSaving = current_time_val.tm_isdst ? RTC_DAYLIGHTSAVING_NONE : RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_StatusTypeDef ret = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  RTC_DateTypeDef sDate = {0};
  sDate.WeekDay = current_time_val.tm_wday+1; // RTC_WEEKDAY_MONDAY;
  sDate.Month = current_time_val.tm_mon;
  sDate.Date = current_time_val.tm_mday;
  sDate.Year = current_time_val.tm_year - 100;
  ret = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void
sntp_client_init(void)
{
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
  sntp_setserver(0, netif_ip_gw4(netif_default));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
  sntp_init();
}
