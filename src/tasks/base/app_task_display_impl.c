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
#include <time.h>

#include "ad9516/dev_auxpll_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ad9548/dev_ad9548_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "cmsis_os.h"
#include "display.h"
#include "display_boards.h"
#include "display_brief.h"
#include "display_common.h"
#include "display_log.h"
#include "display_menu.h"
#include "display_sensors.h"
#include "display_tasks.h"
#include "fpga/dev_fpga_print.h"
#include "menu/menu.h"
#include "powermon/dev_powermon_display.h"
#include "rtc_util.h"
#include "sfpiic/dev_sfpiic_print.h"
#include "thset/dev_thset_print.h"
#include "vxsiicm/dev_vxsiicm_print.h"

const uint32_t DISPLAY_REFRESH_TIME_MS = 2000;
const uint32_t DISPLAY_REPAINT_TIME_MS = 10000;
const uint32_t DISPLAY_RESET_TIME_MS = 60000; // full reset terminal
static uint32_t displayUpdateCount = 0;

#if defined(BOARD_TDC64) || defined(BOARD_TDC72)
static const bool has_digipots = true;
#else
static const bool has_digipots = false;
#endif

#if ! defined(BOARD_TDC72)
static const bool has_auxpll = true;
#else
static const bool has_auxpll = false;
#endif

#if defined(BOARD_CRU16) || defined(BOARD_TTVXS)
static const bool has_clkmux = true;
#else
static const bool has_clkmux = false;
#endif

#if defined(BOARD_TTVXS)
static const bool has_vxsiicm = true;
#else
static const bool has_vxsiicm = false;
#endif

static const int DISPLAY_SYS_STATUS_Y = 2;
static const int DISPLAY_SYS_STATUS_H = 1;
static const int DISPLAY_POWERMON_Y = (DISPLAY_SYS_STATUS_Y + DISPLAY_SYS_STATUS_H);
static const int DISPLAY_POTS_Y = (0 + DISPLAY_POWERMON_Y + DISPLAY_POWERMON_H);
static const int DISPLAY_POTS_H  = has_digipots ? 1 : 0;
static const int DISPLAY_SENSORS_Y = (0 + DISPLAY_POTS_Y + DISPLAY_POTS_H);
static const int DISPLAY_TEMP_H = 1;
static const int DISPLAY_TEMP_Y = (0 + DISPLAY_SENSORS_Y + DISPLAY_SENSORS_HEIGHT);
static const int DISPLAY_MAIN_Y = (1 + DISPLAY_TEMP_H + DISPLAY_TEMP_Y);
static const int DISPLAY_MAIN_H = has_vxsiicm ? 5 : 4;
static const int DISPLAY_PLL_Y = (0 + DISPLAY_MAIN_Y + DISPLAY_MAIN_H);
#if ENABLE_AD9545
static const int DISPLAY_PLL_H = AD9545_DISPLAY_PLL_H;
static const int DISPLAY_PLL_DETAIL_H = AD9545_DISPLAY_PLL_DETAIL_H;
#elif ENABLE_AD9548
static const int DISPLAY_PLL_H = AD9548_DISPLAY_PLL_H;
static const int DISPLAY_PLL_DETAIL_H = AD9548_DISPLAY_PLL_DETAIL_H;
#else
static const int DISPLAY_PLL_H = 0;
static const int DISPLAY_PLL_DETAIL_H = 0;
#endif
static const int DISPLAY_AUXPLL_Y = (0 +DISPLAY_PLL_Y + DISPLAY_PLL_H);
static const int DISPLAY_AUXPLL_H = has_auxpll ? 1 : 0;
static const int DISPLAY_LOG_Y = (1 + DISPLAY_AUXPLL_Y + DISPLAY_AUXPLL_H);
static const int DISPLAY_AUXPLL_DETAIL_Y = (DISPLAY_PAGE_Y + DISPLAY_PLL_DETAIL_H + 1);

static void print_main(int y)
{
    print_goto(y, 1);
#ifdef ENABLE_SFPIIC
    dev_sfpiic_print();
#endif
    dev_eeprom_config_print();
    if (has_vxsiicm)
        dev_vxsiicm_print();
    if (has_clkmux)
        print_clkmux();
    print_fpga();
}

static void display_summary_page(int y, bool repaint)
{
    print_goto(y, 1);

    print_system_status(DISPLAY_SYS_STATUS_Y);
    print_powermon(DISPLAY_POWERMON_Y);
    if (has_digipots)
        print_digipots();

    print_sensors(DISPLAY_SENSORS_Y); // always repaint

    print_goto(DISPLAY_TEMP_Y, 1);
    print_thset_line(); // always repaint
    printf("\n");
    print_main(DISPLAY_MAIN_Y);
    if (DISPLAY_PLL_Y + DISPLAY_PLL_H < screen_height-1)
    print_pll(DISPLAY_PLL_Y);
    if (has_auxpll && DISPLAY_AUXPLL_Y + DISPLAY_AUXPLL_H < screen_height-1)
        print_auxpll(DISPLAY_AUXPLL_Y);

    printf("\n");
    print_log_messages(DISPLAY_LOG_Y, screen_height-1-DISPLAY_LOG_Y, repaint);
}

void clear_page_contents(void)
{
    print_clearbox(DISPLAY_PAGE_Y, screen_height-DISPLAY_PAGE_Y);
}

void display_page_contents(display_mode_t mode, bool repaint)
{
    static display_mode_t old_mode;
    if (old_mode != mode) {
        repaint = true;
        old_mode = mode;
    }
    if (repaint)
        display_clear_page();

    switch (display_mode) {
    case DISPLAY_MENU:
        display_menu_page(DISPLAY_PAGE_Y, repaint);
        break;
    case DISPLAY_SUMMARY:
        display_summary_page(DISPLAY_PAGE_Y, repaint);
        break;
    case DISPLAY_LOG:
        display_log_page(DISPLAY_PAGE_Y, screen_height-DISPLAY_PAGE_Y-2, repaint);
        break;
    case DISPLAY_DIGIPOT:
#ifdef ENABLE_DIGIPOTS
        display_digipots_page(DISPLAY_PAGE_Y, repaint);
#endif
        break;
    case DISPLAY_PLL_DETAIL:
        display_pll_detail(DISPLAY_PAGE_Y);
        display_auxpll_detail(DISPLAY_AUXPLL_DETAIL_Y);
        break;
    case DISPLAY_BOARDS:
        display_boards_page(DISPLAY_PAGE_Y, repaint);
        break;
    case DISPLAY_SFP_DETAIL:
#ifdef ENABLE_SFPIIC
        display_sfpiic_page(DISPLAY_PAGE_Y);
#endif
        break;
    case DISPLAY_TASKS:
        display_tasks_page(DISPLAY_PAGE_Y);
        break;
    case DISPLAY_DEVICES:
        display_devices_page(DISPLAY_PAGE_Y, repaint);
        break;
    case DISPLAY_MODE_COUNT:
        break;
    default:
        break;
    }
}

static void print_prompt(void)
{
    print_goto(screen_height-1, 1);
    printf("> %s", ANSI_CLEAR_EOL);
    printf(ANSI_SHOW_CURSOR);
}

uint32_t display_refresh_tick = 0;
uint32_t display_repaint_tick = 0;
uint32_t display_reset_tick = 0;
static struct tm old_tm = {0};

void display_task_run(void)
{
    const uint32_t now = osKernelSysTick();
    if (now > display_refresh_tick + DISPLAY_REFRESH_TIME_MS) {
        schedule_display_refresh();
    }
    if (now > display_repaint_tick + DISPLAY_REPAINT_TIME_MS) {
        schedule_display_repaint();
    }
    if (now > display_reset_tick + DISPLAY_RESET_TIME_MS) {
        schedule_display_reset();
    }
    struct tm tm = {0};
    get_rtc_tm(&tm);
    const bool time_updated = old_tm.tm_sec != tm.tm_sec;
    old_tm = tm;

    const bool reset_flag = read_display_reset();
    const bool repaint_flag = reset_flag || read_display_repaint();
    const bool refresh_flag = repaint_flag || read_display_refresh();
    bool idle = !time_updated && !refresh_flag;

    if (idle)
        return;

    if (reset_flag) {
        print_get_screen_size();
        printf(ANSI_CLEARTERM);
    }
    if (repaint_flag) {
        print_get_screen_size();
    }

    if (refresh_flag)
        display_refresh_tick = now;
    if (repaint_flag)
        display_repaint_tick = now;
    if (reset_flag)
        display_reset_tick = now;

    if (! screen_size_set) {
        print_get_screen_size();
    }

    printf(ANSI_CLEAR ANSI_NORM ANSI_HIDE_CURSOR);

    if (refresh_flag) {
        print_header_line(repaint_flag);
    }
    if (time_updated || refresh_flag) {
        print_clock();
    }
    if (refresh_flag) {
        print_footer(repaint_flag);
        print_goto(2, 1);
        display_page_contents(display_mode, repaint_flag);
        // print_get_screen_size();
        // print_footer(repaint_flag);
    }
    print_prompt();
    displayUpdateCount++;
}

void display_task_init(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    init_menu();
}
