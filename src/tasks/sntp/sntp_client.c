/*
**    Copyright 2019-2021 Ilja Slepnev
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

#include "sntp_client.h"

#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "log/log.h"
#include "lwip/opt.h"
#include "lwip/apps/sntp.h"
#include "lwip/netif.h"
#include "rtc_util.h"
#include "cmsis_os.h"

static const double TIME_JUMP_THRESHOLD = 1.0;
static const double TIME_ADJUST_THRESHOLD = 0.1;
static bool time_is_set = false;

static bool set_timeval(const struct timeval *tv)
{
    struct timeval rtc_tv;
    if (!get_rtc_tv(&rtc_tv))
        return false;

    double dt = (tv->tv_sec - rtc_tv.tv_sec) + 1e-6 * (tv->tv_usec - rtc_tv.tv_usec);
    if (time_is_set && (fabs(dt) >= TIME_ADJUST_THRESHOLD)) {
        if (fabs(dt) > TIME_JUMP_THRESHOLD)
            log_printf(LOG_WARNING, "SNTP: time jump by %.3f seconds", dt);
        else
            log_printf(LOG_INFO, "SNTP: time adjust by %.3f seconds", dt);
    }
    struct timeval set_tv = *tv;
    int delay = 1000 - tv->tv_usec / 1000;
    if (delay > 0) {
        set_tv.tv_sec += 1;
        osDelay(delay);
    }
    bool ok = set_rtc_tv(&set_tv);
    if (ok) {
        if (!time_is_set) {
            const time_t t = tv->tv_sec;
            struct tm tm;
            if (0 == localtime_r(&t, &tm))
                return false;
            char buf[32] = {0};
            strftime(buf, sizeof(buf) - 1, "%d.%m.%Y %H:%M:%S", &tm);
            log_printf(LOG_INFO, "SNTP: time set %s", buf);
        }
        time_is_set = true;
    }
    return ok;
}

void sntp_client_set_system_time(unsigned int sec, unsigned int microsec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = microsec;
    set_timeval(&tv);
}

void sntp_client_init(void)
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
