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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "app_task_display_impl.h"

#include "ad9516/dev_auxpll_print.h"
#include "ad9516/dev_auxpll_types.h"
#include "ad9545/ad9545_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp_powermon.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "dev_common_types.h"
#include "dev_mcu.h"
#include "thset/dev_thset_print.h"
#include "devices.h"
#include "devices_types.h"
#include "display.h"
#include "display_boards.h"
#include "display_common.h"
#include "display_log.h"
#include "display_tasks.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "fpga/dev_fpga_print.h"
#include "fpga/dev_fpga_types.h"
#include "logbuffer.h"
#include "logentry.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_display.h"
#include "powermon/dev_powermon_types.h"
#include "rtc_util.h"
#include "sfpiic/dev_sfpiic_print.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rtc.h"
#include "system_status.h"
#include "ttvxs_clkmux/dev_ttvxs_clkmux_types.h"
#include "version.h"

const uint32_t DISPLAY_REFRESH_TIME_MS = 1000;
static uint32_t displayUpdateCount = 0;
static int force_refresh = 0;

static void devPrintStatus(void)
{
    dev_sfpiic_print();
    dev_eeprom_config_print();
    const Dev_vxsiicm *vxsiicm= get_dev_vxsiicm();
    printf("VXS I2C:        %d boards %s", get_vxsiic_board_count(vxsiicm), deviceStatusResultStr(vxsiicm->dev.device_status));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static const char *auxpllStateStr(AuxPllState state)
{
    switch(state) {
    case AUXPLL_STATE_INIT:    return "INIT";
    case AUXPLL_STATE_RESET:    return "RESET";
    case AUXPLL_STATE_SETUP:     return ANSI_GREEN  "SETUP"     ANSI_CLEAR;
    case AUXPLL_STATE_RUN:   return ANSI_GREEN    "RUN"   ANSI_CLEAR;
    case AUXPLL_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case AUXPLL_STATE_FATAL:   return ANSI_RED    "FATAL"   ANSI_CLEAR;
    default: return "?";
    }
}

#define DISPLAY_SYS_STATUS_Y 2
#define DISPLAY_SYS_STATUS_H 1
#define DISPLAY_POWERMON_Y (DISPLAY_SYS_STATUS_Y + DISPLAY_SYS_STATUS_H)
#define DISPLAY_SENSORS_Y (0 + DISPLAY_POWERMON_Y + DISPLAY_POWERMON_H)
#define DISPLAY_TEMP_H 1
#define DISPLAY_TEMP_Y (0 + DISPLAY_SENSORS_Y + DISPLAY_SENSORS_H)
#define DISPLAY_MAIN_Y (0 + DISPLAY_TEMP_H + DISPLAY_TEMP_Y)
#define DISPLAY_MAIN_H 3
#define DISPLAY_CLKMUX_Y (0 + DISPLAY_MAIN_Y + DISPLAY_MAIN_H)
#define DISPLAY_CLKMUX_H 1
#define DISPLAY_FPGA_Y (0 + DISPLAY_CLKMUX_Y + DISPLAY_CLKMUX_H)
#define DISPLAY_FPGA_H 1
#define DISPLAY_PLL_Y (0 + DISPLAY_FPGA_Y + DISPLAY_FPGA_H)
#define DISPLAY_PLL_H 5
#define DISPLAY_AUXPLL_Y (0 +DISPLAY_PLL_Y + DISPLAY_PLL_H)
#define DISPLAY_AUXPLL_H 2
#define DISPLAY_LOG_Y (1 + DISPLAY_AUXPLL_Y + DISPLAY_AUXPLL_H)

#define DISPLAY_TASKS_Y 2
#define DISPLAY_BOARDS_Y 3

#define DISPLAY_PLL_DETAIL_Y 2
#define DISPLAY_AUXPLL_DETAIL_Y (DISPLAY_PLL_DETAIL_Y + DISPLAY_PLL_DETAIL_H + 1)
#define DISPLAY_AUXPLL_DETAIL_H 3

static void print_footer(void)
{
    print_goto(screen_height, 1);
    print_footer_line();
}

static void print_system_status(void)
{
    print_goto(DISPLAY_SYS_STATUS_Y, 1);
    const SensorStatus systemStatus = getSystemStatus();
    printf("System status: %s",
           sensor_status_ansi_str(systemStatus));
    print_clear_eol();

}

static void print_powermon(void)
{
    print_goto(DISPLAY_POWERMON_Y, 1);
    print_powermon_box();
}

static void print_sensors(void)
{
    const PmState pmState = get_powermon_state();
    if (pmState == PM_STATE_INIT) {
        print_clearbox(DISPLAY_SENSORS_Y, DISPLAY_SENSORS_H);
    } else {
        print_goto(DISPLAY_SENSORS_Y, 1);
        print_sensors_box();
    }
}

static void print_thset(void)
{
    print_goto(DISPLAY_TEMP_Y, 1);
    print_thset_box();
}

static void print_main(void)
{
    print_goto(DISPLAY_MAIN_Y, 1);
//    if (getMainState() == MAIN_STATE_RUN) {
//        printf("Main state:     %s", mainStateStr(getMainState()));
        print_clear_eol();
        devPrintStatus();
//        printf("%s\n", ANSI_CLEAR_EOL);
//    } else {
//        print_clearbox(DISPLAY_MAIN_Y, DISPLAY_MAIN_H);
//    }
}

static void print_ttvxs_clkmux(void)
{
    print_goto(DISPLAY_CLKMUX_Y, 1);
    printf("CLKMUX");
    printf("%s", sensor_status_ansi_str(get_clkmux_sensor_status()));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_fpga(void)
{
    print_goto(DISPLAY_FPGA_Y, 1);
    dev_fpga_print_box();
}

static void print_pll(void)
{
    print_goto(DISPLAY_PLL_Y, 1);
    dev_ad9545_print_box();
}

static void print_auxpll(void)
{
    print_goto(DISPLAY_AUXPLL_Y, 1);
    auxpllPrint();
}


static int old_enable_stats_display = 0;

static void display_summary(void)
{
    print_system_status();
    print_powermon();
    if (enable_stats_display) {
        print_sensors();
    }
    print_thset();
    print_main();
    print_ttvxs_clkmux();
    print_fpga();
    print_pll();
    print_auxpll();
    print_log_messages(DISPLAY_LOG_Y, screen_height-1-DISPLAY_LOG_Y);
}

static void display_pll_detail(void)
{
    print_clearbox(DISPLAY_PLL_DETAIL_Y, DISPLAY_PLL_DETAIL_H);
    print_goto(DISPLAY_PLL_DETAIL_Y, 1);
    dev_ad9545_verbose_status();
}

static void display_auxpll_detail(void)
{
    print_clearbox(DISPLAY_AUXPLL_DETAIL_Y, DISPLAY_AUXPLL_DETAIL_H);
    print_goto(DISPLAY_AUXPLL_DETAIL_Y, 1);
    auxpllPrintStatus();
}

static display_mode_t old_display_mode = DISPLAY_NONE;

uint32_t old_tick = 0;
static struct tm old_tm = {0};

void display_task_run(void)
{
    uint32_t tick = osKernelSysTick();
    struct tm tm;
    get_rtc_tm(&tm);
    int time_updated = old_tm.tm_sec != tm.tm_sec;
    if (tick > old_tick + DISPLAY_REFRESH_TIME_MS)
        schedule_display_refresh();
    if (old_display_mode != display_mode)
        schedule_display_refresh();
    int need_refresh = read_display_refresh();
    if (!need_refresh)
        return;
    old_tick = tick;
    old_tm = tm;

    int need_clear_screen =
        display_mode == DISPLAY_NONE ||
        display_mode == DISPLAY_LOG ||
        display_mode == DISPLAY_TASKS ||
        display_mode == DISPLAY_BOARDS ||
        display_mode == DISPLAY_DEVICES;
    if (need_clear_screen) {
        if (old_display_mode != display_mode) {
            printf(ANSI_CLEARTERM);
            printf(ANSI_GOHOME ANSI_CLEAR);
            print_header();
            printf(ANSI_SHOW_CURSOR);
        }
    }
    old_display_mode = display_mode;
    if (display_mode == DISPLAY_NONE)
        return;
    if (enable_stats_display && !old_enable_stats_display) {
        force_refresh = 1;
    }
    old_enable_stats_display = enable_stats_display;
    if (force_refresh) {
        printf(ANSI_CLEARTERM);
        force_refresh = 0;
    }
    printf(ANSI_GOHOME ANSI_CLEAR);
    printf(ANSI_HIDE_CURSOR);
    print_header();
    switch (display_mode) {
    case DISPLAY_SUMMARY:
        display_summary();
        break;
    case DISPLAY_LOG:
        display_log(3, screen_height-1-3);
        break;
    case DISPLAY_PLL_DETAIL:
        display_pll_detail();
        display_auxpll_detail();
        break;
    case DISPLAY_BOARDS:
        display_boards(DISPLAY_BOARDS_Y);
        break;
    case DISPLAY_TASKS:
        display_tasks(DISPLAY_TASKS_Y);
        break;
    case DISPLAY_DEVICES:
        display_devices();
        break;
    case DISPLAY_NONE:
        break;
    default:
        break;
    }
    print_get_screen_size();
    print_footer();
    print_goto(screen_height-1, 1);
    printf(ANSI_SHOW_CURSOR); // show cursor
    printf("%s", ANSI_CLEAR_EOL);
    displayUpdateCount++;
}

void display_task_init(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    printf(ANSI_CLEAR ANSI_CLEARTERM ANSI_GOHOME);
}
