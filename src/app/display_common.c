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

#include "display_common.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "app_tasks.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "devicelist_print.h"
#include "display.h"
#include "freertos_stats.h"
#include "mac_address.h"
#include "rtc_util.h"
#include "stm32_hal.h"
#include "version.h"

SensorStatus get_device_sensor_status(DeviceClass device_class)
{
    const DeviceBase *d = find_device_const(device_class);
    if (!d || !d->priv)
        return SENSOR_UNKNOWN;

    switch (d->device_status) {
    case DEVICE_NORMAL:
        return SENSOR_NORMAL;
    case DEVICE_FAIL:
        return SENSOR_CRITICAL;
    case DEVICE_UNKNOWN:
        return SENSOR_UNKNOWN;
    default:
        return SENSOR_UNKNOWN;
    }
}

const char *device_sensor_status_ansi_str(DeviceClass device_class)
{
    return sensor_status_ansi_str(get_device_sensor_status(device_class));
}

void display_device_sensor_ansi_str(const char *name, DeviceClass device_class)
{
    printf("%s %s", name,
           device_sensor_status_ansi_str(device_class));
    print_clear_eol();
}

void display_clear_page(void)
{
    print_clearbox(2, screen_height - 2);
}

void display_devices_page(int y)
{
    display_clear_page();
    print_goto(y, 1);
    printf("Device list" ANSI_CLEAR_EOL "\n");
    devicelist_print(deviceList.list[0], 0);
    printf(ANSI_CLEAR_EOL);
}

static void print_uptime_str(void)
{
    unsigned int ss = osKernelSysTick() / osKernelSysTickFrequency;
    unsigned int dd = ss / 86400;
    ss -= dd*86400;
    unsigned int hh = ss / 3600;
    ss -= hh*3600;
    unsigned int mm = ss / 60;
    ss -= mm*60;
    if (dd > 1)
        printf("%u days ", dd);
    if (dd == 1)
        printf("%u day ", dd);
    printf("%2u:%02u:%02u", hh, mm, ss);
}

static void print_rtc_str(void)
{
    struct tm tm = {0};
    get_rtc_tm(&tm);
    char buf[32];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &tm);
    printf("%s", buf);
}

void print_clock(void)
{
    int col = screen_width - 37;
    if (col > 60) {
        print_goto(1, col);
        printf("Uptime: ");
        print_uptime_str();
#ifdef HAL_RTC_MODULE_ENABLED
        printf("  ");
        print_rtc_str();
#endif
    }
}

void print_header_line(void)
{
    print_goto(1, 1);
        // Title
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_GRAY ANSI_BGR_BLUE);
#ifdef HAL_ETH_MODULE_ENABLED
    if (tcpipThreadId) {
        uint8_t macaddress[6];
        get_mac_address(macaddress);
        if (app_ipv4 && eth_link_up)
            printf(
                "  IP:%d.%d.%d.%d ",
                app_ipv4 & 0xFF,
                (app_ipv4 >> 8) & 0xFF,
                (app_ipv4 >> 16) & 0xFF,
                (app_ipv4 >> 24) & 0xFF);
        else
            printf(
                " %s %02X:%02X:%02X:%02X:%02X:%02X %s",
                eth_link_up ? "": ANSI_BGR_RED,
                macaddress[0], macaddress[1], macaddress[2],
                macaddress[3], macaddress[4], macaddress[5],
                ANSI_BGR_BLUE);
    }
#endif
    printf("  %u MHz", (unsigned int)(HAL_RCC_GetHCLKFreq()/1000000));
    printf(" %3u%%", (unsigned int)freertos_get_cpu_load_percent());
    printf("     %s" ANSI_BGR_BLUE ANSI_CLEAR_EOL,
           enable_power ? ANSI_BGR_BLUE "           " : ANSI_BGR_RED " Power-OFF ");
    printf(ANSI_CLEAR_EOL);

    print_clock();
    printf(ANSI_CLEAR);
}

enum { MAX_KEYS = 10 };
static const char *keys[10] = {
    "SPACE", "P", "F2", "F3", "F4",
    "F5", "F6", "F7", "F8", "F9"
};
static const char *pages[10] = {
    "Next", "Power", "Home", "VXS", "SFP",
    "PLL", "Adj", "Log", "Tasks", "Devs"
};
static const display_mode_t modes[10] = {
    -1, -1, DISPLAY_SUMMARY, DISPLAY_BOARDS, DISPLAY_SFP_DETAIL,
    DISPLAY_PLL_DETAIL, DISPLAY_DIGIPOT, DISPLAY_LOG, DISPLAY_TASKS, DISPLAY_DEVICES
};

void print_footer_line(void)
{
    print_goto(screen_height, 1);
    printf(ANSI_BGR_BLUE ANSI_GRAY);
    for (int i=0; i<MAX_KEYS; i++) {
        const char *hilight = (modes[i] == display_mode) ? ANSI_BGR_GREEN : ANSI_BGR_BLUE;
        if (i == 1)
            hilight = enable_power ? ANSI_BGR_BLUE : ANSI_BGR_RED;
        printf("%s " ANSI_BOLD "%s " ANSI_NORM "%s " ANSI_BGR_BLUE,
               hilight, keys[i], pages[i]);
    }
    printf(ANSI_CLEAR_EOL ANSI_BGR_BLACK ANSI_CLEAR);
}

void dev_eeprom_config_print(void)
{
    display_device_sensor_ansi_str("EEPROM[config]", DEV_CLASS_EEPROM);
}
