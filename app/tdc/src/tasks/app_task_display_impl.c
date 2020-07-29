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

#include "app_task_display_impl.h"

#include "ad9516/dev_auxpll_print.h"
#include "ad9545/ad9545_print.h"
#include "ad9545/dev_ad9545_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp_powermon.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "dev_common_types.h"
#include "dev_mcu.h"
#include "devices_types.h"
#include "digipot/dev_digipot.h"
#include "display.h"
#include "display_common.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "fpga/dev_fpga_print.h"
#include "fpga/dev_fpga_types.h"
#include "freertos_stats.h"
#include "logbuffer.h"
#include "logentry.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_display.h"
#include "powermon/dev_powermon_types.h"
#include "stm32f7xx_hal.h"
#include "system_status.h"
#include "thset/dev_thset_print.h"
#include "version.h"

const uint32_t DISPLAY_REFRESH_TIME_MS = 1000;
static uint32_t displayUpdateCount = 0;
static int force_refresh = 0;

static void print_pm_pots(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_DIGIPOTS);
    if (!d || !d->priv)
        return;
    const Dev_digipots_priv *priv = (const Dev_digipots_priv *)device_priv_const(d);

    printf("POTS: ");
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        if (priv->pot[i].dev.device_status == DEVICE_NORMAL)
            printf("%3u ", priv->pot[i].priv.value);
        else
            printf("?   ");
    }
    bool Ok = true;
    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        if (priv->pot[0].dev.device_status != DEVICE_NORMAL)
        Ok = false;
    }
    if (Ok) {
        printf(Ok ? STR_RESULT_NORMAL : STR_RESULT_WARNING);
    }
    printf("%s\n", ANSI_CLEAR_EOL);
}


static void print_log_entry(uint32_t index)
{
    struct LogEntry ent;
    log_get(index, &ent);
    const char *prefix = "";
    const char *suffix = ANSI_CLEAR_EOL ANSI_CLEAR;
    switch (ent.priority) {
    case LOG_EMERG: prefix = ANSI_PUR; break;
    case LOG_ALERT: prefix = ANSI_PUR; break;
    case LOG_CRIT: prefix = ANSI_PUR; break;
    case LOG_ERR: prefix = ANSI_RED; break;
    case LOG_WARNING: prefix = ANSI_YELLOW; break;
    case LOG_NOTICE: break;
    case LOG_INFO: break;
    case LOG_DEBUG: break;
    default: prefix = ANSI_PUR; break;
    }
    printf("%s%8ld.%03ld %s%s", prefix,
           ent.tick/1000, ent.tick%1000, ent.str, suffix);
    printf("%s\n", ANSI_CLEAR_EOL);
}

#define DISPLAY_SYS_STATUS_Y 2
#define DISPLAY_SYS_STATUS_H 1
#define DISPLAY_POWERMON_Y (DISPLAY_SYS_STATUS_Y + DISPLAY_SYS_STATUS_H)
#define DISPLAY_POTS_H 1
#define DISPLAY_POTS_Y (0 + DISPLAY_POWERMON_Y + DISPLAY_POWERMON_H)
#define DISPLAY_SENSORS_Y (0 + DISPLAY_POTS_Y + DISPLAY_POTS_H)
#define DISPLAY_TEMP_H 1
#define DISPLAY_TEMP_Y (0 + DISPLAY_SENSORS_Y + DISPLAY_SENSORS_H)
#define DISPLAY_MAIN_Y (0 + DISPLAY_TEMP_H + DISPLAY_TEMP_Y)
#define DISPLAY_MAIN_H 3
#define DISPLAY_FPGA_Y (0 + DISPLAY_MAIN_Y + DISPLAY_MAIN_H)
#define DISPLAY_FPGA_H 1
#define DISPLAY_PLL_Y (0 + DISPLAY_FPGA_Y + DISPLAY_FPGA_H)
#define DISPLAY_AUXPLL_Y (0 +DISPLAY_PLL_Y + DISPLAY_PLL_H)
#define DISPLAY_AUXPLL_H 2
#define DISPLAY_LOG_Y (1 + DISPLAY_AUXPLL_Y + DISPLAY_AUXPLL_H)
#define DISPLAY_LOG_H 5
#define DISPLAY_STATS_Y (0 + DISPLAY_LOG_Y + DISPLAY_LOG_H)

#define DISPLAY_TASKS_Y 2

#define DISPLAY_PLL_DETAIL_Y 2

//#define DISPLAY_HEIGHT (DISPLAY_PLL_Y + DISPLAY_PLL_H)

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

static char statsBuffer[1000];

static void print_header(void)
{
    // Title
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_GRAY ANSI_BGR_BLUE);
    printf(" %lu MHz", HAL_RCC_GetHCLKFreq()/1000000);
    printf(" %lu%%", freertos_get_cpu_load_percent());
    if (display_mode == DISPLAY_NONE) {
        printf("     display refresh paused");
    } else {
        printf("     Uptime: ");
        print_uptime_str();
        printf("     %s%s%s%s%s",
               ANSI_BOLD ANSI_BLINK,
               enable_power ? ANSI_BGR_BLUE "           " : ANSI_BGR_RED " Power-OFF ",
               ANSI_BGR_BLUE " ",
               enable_stats_display? ANSI_BGR_BLUE "               " : ANSI_BGR_RED " Press any key ",
               ANSI_BGR_BLUE);
    }
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
}

static void print_footer(void)
{
    print_goto(screen_height, 1);
    printf(ANSI_BGR_BLUE ANSI_GRAY "Test" ANSI_CLEAR_EOL ANSI_CLEAR);
}

void print_system_status(const Devices *dev)
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

static void print_digipots(void)
{
    print_pm_pots();
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

static void devPrintStatus(const Devices *d)
{
    printf("SFP IIC:   %s", deviceStatusResultStr(d->sfpiic.dev.device_status));
    print_clear_eol();
    dev_eeprom_config_print();
    print_clear_eol();
    //    printf("EEPROM VXS PB: %s", deviceStatusResultStr(d->eeprom_vxspb.dev.device_status));
    //    print_clear_eol();
    //    printf("PLL I2C:       %s", deviceStatusResultStr(d->pll.dev.device_status));
    //    print_clear_eol();
}

static void print_main(const Devices *dev)
{
    print_goto(DISPLAY_MAIN_Y, 1);
//    if (getMainState() == MAIN_STATE_RUN) {
//        printf("Main state:     %s", mainStateStr(getMainState()));
//        print_clear_eol();
        devPrintStatus(dev);
//        printf("%s\n", ANSI_CLEAR_EOL);
//    } else {
//        print_clearbox(DISPLAY_MAIN_Y, DISPLAY_MAIN_H);
//    }
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

static void print_log_lines(int count)
{
    uint32_t max_count = count;
    if (max_count > LOG_BUF_SIZE)
        max_count = LOG_BUF_SIZE;
    volatile const uint32_t log_count = log_get_count();
    volatile const uint32_t log_wptr = log_get_wptr();
    volatile const uint32_t log_start = (log_count > max_count) ? (log_wptr + LOG_BUF_SIZE - max_count) % LOG_BUF_SIZE : 0;
    if (log_start <= log_wptr) {
        for (uint32_t i=log_start; i<log_wptr; i++)
            print_log_entry(i);
    } else {
        for (uint32_t i=log_start; i<LOG_BUF_SIZE; i++)
            print_log_entry(i);
        for (uint32_t i=0; i<log_wptr; i++)
            print_log_entry(i);
    }
    for (uint32_t i=log_count; i<max_count; i++)
        print_clear_eol();
}

static void print_log_messages(void)
{
    //    print_clearbox(DISPLAY_LOG_Y, DISPLAY_LOG_H);
        print_goto(DISPLAY_LOG_Y, 1);
        if (screen_height > DISPLAY_LOG_Y + 1)
            print_log_lines(screen_height - 1 - DISPLAY_LOG_Y);
}

static void display_log(void)
{
    print_goto(2, 1);
    printf("Log messages\n" ANSI_CLEAR_EOL);
    print_goto(3, 1);
    if (screen_height > 3 + 1)
        print_log_lines(screen_height - 1 - 3);
}

static void display_pot(const Devices * dev)
{
    print_goto(2, 1);
    printf("Voltage adjustments\n" ANSI_CLEAR_EOL);
    printf("  Keys: UP, DOWN: select channel; +, -: adjust voltage; 0: reset; w: write eeprom\n" ANSI_CLEAR_EOL);
    print_goto(4, 1);
    printf("\n");
    printf("   adjustment ");
    pm_sensor_print_header();
    const DeviceBase *dev_pm = find_device_const(DEV_CLASS_POWERMON);
    const Dev_powermon_priv *pm = dev_pm ? (Dev_powermon_priv *)device_priv_const(dev_pm) : 0;

    const DeviceBase *dev_dp = find_device_const(DEV_CLASS_DIGIPOTS);
    if (!dev_dp || !dev_dp->priv)
        return;
    const Dev_digipots_priv *dp = (const Dev_digipots_priv *)device_priv_const(dev_dp);

    for (int i=0; i<DEV_DIGIPOT_COUNT; i++) {
        const Dev_ad5141 *p = &dp->pot[i];
        printf(" %s %s  ", (i == digipot_screen_selected) ? ">" : " ", potLabel((PotIndex)(i)));
        if (p->dev.device_status == DEVICE_NORMAL)
            printf("%3u ", p->priv.value);
        else
            printf("?   ");
        if (pm && (int)p->priv.sensorIndex > 0 && (int)p->priv.sensorIndex < POWERMON_SENSORS) {
            const pm_sensor *sensor = &pm->sensors[p->priv.sensorIndex];
            const int isOn = monIsOn(pm->sw_state, p->priv.sensorIndex);
            printf("%10s", sensor->priv.label);
            pm_sensor_print_values(sensor, isOn);
        } else {
            printf("<no sensor>");
        }
        printf("%s\n", ANSI_CLEAR_EOL);
    }
//    pot_debug();
}

static int old_enable_stats_display = 0;

static void display_summary(const Devices * dev)
{
    print_system_status(dev);
    print_powermon();
    print_digipots();
    if (enable_stats_display) {
        print_sensors();
    }
    print_thset();
    print_main(dev);
    print_fpga();
    print_pll();
#ifdef BOARD_TDC64
    print_auxpll();
#endif
    print_log_messages();
}

static void display_tasks(void)
{
    print_clearbox(DISPLAY_TASKS_Y, uxTaskGetNumberOfTasks());
    print_goto(DISPLAY_TASKS_Y, 1);
    printf("FreeRTOS %s, CMSIS %u.%u, CMSIS-OS %u.%u", tskKERNEL_VERSION_NUMBER,
           __CM_CMSIS_VERSION >> 16, __CM_CMSIS_VERSION & 0xFFFF,
           osCMSIS >> 16, osCMSIS & 0xFFFF
           );
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
    char *buf = statsBuffer;
    strcpy(buf, "Task");
    buf += strlen(buf);
    for(int i = strlen("Task"); i < ( configMAX_TASK_NAME_LEN - 3 ); i++) {
        *buf = ' ';
        buf++;
        *buf = '\0';
    }
    const char *hdr = "  Abs Time      % Time\r\n****************************************" ANSI_CLEAR_EOL "\n";
    strcpy(buf, hdr);
    buf += strlen(hdr);
    vTaskGetRunTimeStats(buf);
    printf("%s", statsBuffer);
    printf(ANSI_CLEAR_EOL);
}

static void display_pll_detail(const Devices * dev)
{
    print_clearbox(DISPLAY_PLL_DETAIL_Y, DISPLAY_PLL_DETAIL_H);
    print_goto(DISPLAY_PLL_DETAIL_Y, 1);
    dev_ad9545_verbose_status();
}

static display_mode_t old_display_mode = DISPLAY_NONE;

uint32_t old_tick = 0;

void display_task_run(void)
{
    uint32_t tick = osKernelSysTick();
    if (tick > old_tick + DISPLAY_REFRESH_TIME_MS)
        schedule_display_refresh();
    if (old_display_mode != display_mode)
        schedule_display_refresh();
    int need_refresh = read_display_refresh();
    if (!need_refresh)
        return;
    old_tick = tick;

    const Devices * d = getDevices();
    int need_clear_screen =
        display_mode == DISPLAY_NONE ||
        display_mode == DISPLAY_LOG ||
        display_mode == DISPLAY_DIGIPOT ||
        display_mode == DISPLAY_TASKS ||
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
        display_summary(d);
        break;
    case DISPLAY_LOG:
        display_log();
        break;
    case DISPLAY_DIGIPOT:
        display_pot(d);
        break;
    case DISPLAY_PLL_DETAIL:
        display_pll_detail(d);
        break;
    case DISPLAY_TASKS:
        display_tasks();
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
