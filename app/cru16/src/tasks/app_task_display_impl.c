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

#include "app_task_display_impl.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ad9516/dev_auxpll_print.h"
#include "ad9516/dev_auxpll_types.h"
#include "ad9545/ad9545_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp_powermon.h"
#include "cmsis_os.h"
#include "cru16_clkmux/dev_cru16_clkmux.h"
#include "cru16_clkmux/dev_cru16_clkmux_types.h"
#include "debug_helpers.h"
#include "dev_common_types.h"
#include "dev_mcu.h"
#include "devices_types.h"
#include "digipot/dev_digipot.h"
#include "dev_digipot_print.h"
#include "display.h"
#include "display_boards.h"
#include "display_brief.h"
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
#include "thset/dev_thset_print.h"
#include "version.h"

const uint32_t DISPLAY_REFRESH_TIME_MS = 1000;
const uint32_t DISPLAY_REPAINT_TIME_MS = 10000;
static uint32_t displayUpdateCount = 0;
static int force_refresh = 0;

static void devPrintStatus(void)
{
    dev_sfpiic_print();
    dev_eeprom_config_print();
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

static int old_enable_stats_display = 0;

static void display_summary(void)
{
    print_system_status(DISPLAY_SYS_STATUS_Y);
    print_powermon(DISPLAY_POWERMON_Y);
    if (enable_stats_display) {
        print_sensors();
    }
    print_thset(DISPLAY_TEMP_Y);
    print_main();
    print_clkmux(DISPLAY_CLKMUX_Y);
    print_fpga(DISPLAY_FPGA_Y);
    print_pll(DISPLAY_PLL_Y);
    print_auxpll(DISPLAY_AUXPLL_Y);
    print_log_messages(DISPLAY_LOG_Y, screen_height-1-DISPLAY_LOG_Y);
}


uint32_t old_tick = 0;
static struct tm old_tm = {0};

void display_task_run(void)
{
    uint32_t tick = osKernelSysTick();
    struct tm tm;
    get_rtc_tm(&tm);
    // int time_updated = old_tm.tm_sec != tm.tm_sec;
    if (tick > old_tick + DISPLAY_REFRESH_TIME_MS)
        schedule_display_refresh();
    const bool repaint_flag = read_display_repaint();
    const bool refresh_flag = repaint_flag || read_display_refresh();
    if (!refresh_flag)
        return;
    old_tick = tick;
    old_tm = tm;

    if (repaint_flag) {
            printf(ANSI_CLEARTERM);
            printf(ANSI_GOHOME ANSI_CLEAR);
            print_header();
            printf(ANSI_SHOW_CURSOR);
    }
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
    case DISPLAY_DIGIPOT:
        display_digipots();
        break;
    case DISPLAY_PLL_DETAIL:
        display_pll_detail(DISPLAY_PLL_DETAIL_Y);
        display_auxpll_detail(DISPLAY_AUXPLL_DETAIL_Y);
        break;
    case DISPLAY_BOARDS:
        display_boards(DISPLAY_BOARDS_Y);
        break;
    case DISPLAY_SFP_DETAIL:
        sfpPrintStatus();
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
    print_get_screen_size();
    printf(ANSI_CLEAR ANSI_CLEARTERM ANSI_GOHOME);
}
