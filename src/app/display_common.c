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
#include <time.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "cmsis_os.h"
#include "devicelist.h"
#include "devicelist_print.h"
#include "display.h"
#include "freertos_stats.h"
#include "mac_address.h"
#include "rtc_util.h"
#include "stm32f7xx_hal.h"
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

void display_devices(void)
{
    print_goto(2, 1);
    printf("Device list" ANSI_CLEAR_EOL "\n");
    devicelist_print(deviceList.list[0], 0);
    printf(ANSI_CLEAR_EOL);
}

static void print_uptime_str(void)
{
    uint32_t ss = osKernelSysTick() / osKernelSysTickFrequency;
    uint16_t dd = ss / 86400;
    ss -= dd*86400;
    uint16_t hh = ss / 3600;
    ss -= hh*3600;
    uint16_t mm = ss / 60;
    ss -= mm*60;
    if (dd > 1)
        printf("%u days ", dd);
    if (dd == 1)
        printf("%u day ", dd);
    printf("%2u:%02u:%02lu", hh, mm, ss);
}

static void print_rtc_str(void)
{
    struct tm tm;
    get_rtc_tm(&tm);
    char buf[32];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &tm);
    printf("%s", buf);
}

void print_header_tdc(void)
{
    // Title
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_GRAY ANSI_BGR_BLUE);
    printf(" %lu MHz", HAL_RCC_GetHCLKFreq()/1000000);
    printf(" %lu%%", freertos_get_cpu_load_percent());

    printf("     Uptime: ");
    print_uptime_str();
    printf("     %s%s%s%s%s",
           ANSI_BOLD ANSI_BLINK,
           enable_power ? ANSI_BGR_BLUE "           " : ANSI_BGR_RED " Power-OFF ",
           ANSI_BGR_BLUE " ",
           enable_stats_display? ANSI_BGR_BLUE "               " : ANSI_BGR_RED " Press any key ",
           ANSI_BGR_BLUE);

    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
}

void print_header(void)
{
    // Title
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_GRAY ANSI_BGR_BLUE);
#ifdef HAL_ETH_MODULE_ENABLED
    uint8_t macaddress[6];
    get_mac_address(macaddress);
    printf("  MAC:%02X:%02X:%02X:%02X:%02X:%02X", macaddress[0], macaddress[1], macaddress[2], macaddress[3], macaddress[4], macaddress[5]);
#endif
    printf(" %lu MHz", HAL_RCC_GetHCLKFreq()/1000000);
    printf(" %lu%%", freertos_get_cpu_load_percent());
    if (display_mode == DISPLAY_NONE) {
        printf("     display refresh paused");
    } else {
        printf("     Uptime: ");
        print_uptime_str();
#ifdef HAL_RTC_MODULE_ENABLED
        printf("   ");
        print_rtc_str();
#endif
        printf("     %s%s%s%s%s",
               ANSI_BOLD ANSI_BLINK,
               enable_power ? ANSI_BGR_BLUE "           " : ANSI_BGR_RED " Power-OFF ",
               ANSI_BGR_BLUE " ",
               enable_stats_display? ANSI_BGR_BLUE "               " : ANSI_BGR_RED " Press any key ",
               ANSI_BGR_BLUE);
    }
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
}

void print_footer_line(void)
{
    printf(ANSI_BGR_BLUE ANSI_GRAY "SPACE: next page   P: switch power" ANSI_CLEAR_EOL ANSI_CLEAR);
}

void dev_eeprom_config_print(void)
{
    display_device_sensor_ansi_str("EEPROM[config]", DEV_CLASS_EEPROM);
}
