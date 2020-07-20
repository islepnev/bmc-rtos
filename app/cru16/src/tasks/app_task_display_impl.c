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

#include "ad9545/ad9545_print.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp_display.h"
#include "bsp_powermon.h"
#include "debug_helpers.h"
#include "dev_auxpll_print.h"
#include "dev_common_types.h"
#include "dev_fpga_types.h"
#include "dev_mcu.h"
#include "dev_pm_sensors_types.h"
#include "dev_powermon.h"
#include "dev_powermon_types.h"
#include "dev_sfpiic_print.h"
#include "dev_thset.h"
#include "dev_thset_types.h"
#include "devices_types.h"
#include "display.h"
#include "logbuffer.h"
#include "logentry.h"
#include "version.h"

#include "rtos/freertos_stats.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rtc.h"
#include "rtc_util.h"
#include "ethernetif.h"

const uint32_t DISPLAY_REFRESH_TIME_MS = 1000;
static uint32_t displayUpdateCount = 0;
static int force_refresh = 0;

static const char *sensor_status_str(SensorStatus status)
{
    switch (status) {
    case SENSOR_UNKNOWN:
        return ANSI_GRAY   "none" ANSI_CLEAR;
    case SENSOR_NORMAL:
        return ANSI_GREEN  "NORM" ANSI_CLEAR;
    case SENSOR_WARNING:
        return ANSI_YELLOW "WARN" ANSI_CLEAR;
    case SENSOR_CRITICAL:
        return ANSI_RED    "CRIT" ANSI_CLEAR;
    default:
        return "????";
    }
}

static const char *deviceStatusResultStr(DeviceStatus status)
{
    switch (status) {
    case DEVICE_UNKNOWN:
        return STR_RESULT_UNKNOWN;
    case DEVICE_NORMAL:
        return STR_RESULT_NORMAL;
    case DEVICE_FAIL:
        return STR_RESULT_FAIL;
    default:
        return "???";
    }
}

static void devPrintStatus(const struct Devices *d)
{
    printf("SFP I2C mux:    %s", deviceStatusResultStr(d->sfpiic.present));
    printf("%s\n", ANSI_CLEAR_EOL);
//    printf("VXS I2C:        %d boards %s", get_vxsiic_board_count(&d->vxsiic), deviceStatusResultStr(d->vxsiic.present));
//    printf("%s\n", ANSI_CLEAR_EOL);
    printf("EEPROM config:  %s", deviceStatusResultStr(d->eeprom_config.present));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static const char *pmStateStr(PmState state)
{
    switch(state) {
    case PM_STATE_INIT:    return "INIT";
    case PM_STATE_WAITINPUT: return "WAIT-INPUT";
    case PM_STATE_STANDBY: return "STANDBY";
    case PM_STATE_RAMP:    return ANSI_YELLOW "RAMP"    ANSI_CLEAR;
    case PM_STATE_RUN:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case PM_STATE_OFF:     return ANSI_GRAY   "OFF"     ANSI_CLEAR;
    case PM_STATE_PWRFAIL: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case PM_STATE_FAILWAIT: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case PM_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case PM_STATE_OVERHEAT: return ANSI_RED    "OVERHEAT" ANSI_CLEAR;
    default: return "?";
    }
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

static void pm_sensor_print_header(void)
{
    printf("%10s %6s %6s %6s %5s", "sensor ", "  V  ", "  A  ", " A max ", "  W  ");
    printf(ANSI_CLEAR_EOL "\n");
}

static void pm_sensor_print_values(const pm_sensor *d, int isOn)
{
    SensorStatus sensorStatus = pm_sensor_status(d);
    int offvoltage = !isOn && (d->busVoltage > 0.1);
    const char *color = "";
    switch (sensorStatus) {
    case SENSOR_UNKNOWN:  color = d->isOptional ? ANSI_GRAY : ANSI_YELLOW; break;
    case SENSOR_NORMAL:   color = ANSI_GREEN;  break;
    case SENSOR_WARNING:  color = ANSI_YELLOW; break;
    case SENSOR_CRITICAL: color = d->isOptional ? ANSI_YELLOW : ANSI_RED;    break;
    }
    printf("%s % 6.3f%s", isOn ? color : offvoltage ? ANSI_YELLOW : "", d->busVoltage, ANSI_CLEAR);
    if (d->shuntVal > SENSOR_MINIMAL_SHUNT_VAL) {
        int backfeed = (d->current < -0.010);
        printf("%s % 6.3f %s% 6.3f % 5.1f", backfeed ? ANSI_YELLOW : "", d->current, backfeed ? ANSI_CLEAR : "", d->currentMax, d->power);
    } else {
        printf("         ");
    }
    //        double sensorStateDuration = pm_sensor_get_sensorStatus_Duration(d) / getTickFreqHz();
}

static void pm_sensor_print(const pm_sensor *d, int isOn)
{
    printf("%10s", d->label);
    if (d->deviceStatus == DEVICE_NORMAL) {
        pm_sensor_print_values(d, isOn);
        printf(" %s", isOn ? sensor_status_ansi_str(d->sensorStatus) : STR_RESULT_OFF);
    } else {
        printf(" %s", STR_RESULT_UNKNOWN);
    }
}

void monPrintValues(const Dev_powermon *d)
{
    pm_sensor_print_header();
    {
        for (int i=0; i<POWERMON_SENSORS; i++) {
            pm_sensor_print(&d->sensors[i], monIsOn(d->sw, (SensorIndex)i));
            printf("%s\n", ANSI_CLEAR_EOL);
        }
    }
}

void auxpllPrint(const Dev_auxpll *d)
{
    printf("PLL AD9516:   %s %s",
           auxpllStateStr(d->fsm_state),
           sensor_status_ansi_str(get_auxpll_sensor_status(d)));
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("  ");
    auxpllPrintStatus(d);
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

#define DISPLAY_POWERMON_Y 2
#define DISPLAY_POWERMON_H 4
#define DISPLAY_SENSORS_Y (0 + DISPLAY_POWERMON_Y + DISPLAY_POWERMON_H)
#define DISPLAY_SENSORS_H (POWERMON_SENSORS + 3)
#define DISPLAY_MAIN_Y (0 + DISPLAY_SENSORS_Y + DISPLAY_SENSORS_H)
#define DISPLAY_MAIN_H 4
#define DISPLAY_CLKMUX_Y (0 + DISPLAY_MAIN_Y + DISPLAY_MAIN_H)
#define DISPLAY_CLKMUX_H 1
#define DISPLAY_FPGA_Y (0 + DISPLAY_CLKMUX_Y + DISPLAY_CLKMUX_H)
#define DISPLAY_FPGA_H 1
#define DISPLAY_PLL_Y (0 + DISPLAY_FPGA_Y + DISPLAY_FPGA_H)
#define DISPLAY_PLL_H 5
#define DISPLAY_AUXPLL_Y (0 +DISPLAY_PLL_Y + DISPLAY_PLL_H)
#define DISPLAY_AUXPLL_H 2
#define DISPLAY_LOG_Y (1 + DISPLAY_AUXPLL_Y + DISPLAY_AUXPLL_H)
#define DISPLAY_LOG_H 5

#define DISPLAY_TASKS_Y 2

#define DISPLAY_PLL_DETAIL_Y 2
#define DISPLAY_PLL_DETAIL_H 25
#define DISPLAY_AUXPLL_DETAIL_Y (DISPLAY_PLL_DETAIL_Y + DISPLAY_PLL_DETAIL_H + 1)
#define DISPLAY_AUXPLL_DETAIL_H 3

#define DISPLAY_HEIGHT 30 // (DISPLAY_PLL_Y + DISPLAY_PLL_H)

static void print_goto(int line, int col)
{
    printf("\x1B[%d;%dH", line, col);
}

static void print_clearbox(int line1, int height)
{
    for(int i=line1; i<line1+height; i++)
        printf("\x1B[%d;H\x1B[K", i);
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
static char statsBuffer[1000];

static void print_header(void)
{
    // Title
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_GRAY ANSI_BGR_BLUE);
    uint8_t macaddress[6];
    get_mac_address(macaddress);
    printf("  MAC:%02X:%02X:%02X:%02X:%02X:%02X", macaddress[0], macaddress[1], macaddress[2], macaddress[3], macaddress[4], macaddress[5]);
    printf(" %lu MHz", HAL_RCC_GetHCLKFreq()/1000000);
    printf(" %lu%%", freertos_get_cpu_load_percent());
    if (display_mode == DISPLAY_NONE) {
        printf("     display refresh paused");
    } else {
        printf("     Uptime: ");
        print_uptime_str();
        printf("   ");
        print_rtc_str();
        printf("     %s%s%s%s%s",
               ANSI_BOLD ANSI_BLINK,
               enable_power ? ANSI_BGR_BLUE "           " : ANSI_BGR_RED " Power-OFF ",
               ANSI_BGR_BLUE " ",
               enable_stats_display? ANSI_BGR_BLUE "               " : ANSI_BGR_RED " Press any key ",
               ANSI_BGR_BLUE);
    }
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
}

static void print_pm_switches(const pm_switches sw)
{
    printf("Switch: ");
    for (int i=0; i<POWER_SWITCH_COUNT; i++) {
        printf("%s %s   ", psw_label(i), sw[i] ? STR_ON : STR_OFF);
    }
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void pm_pgood_print(const pm_pgoods pgood)
{
    //    printf("Live insert: %s", pm.vmePresent ? STR_RESULT_ON : STR_RESULT_OFF);
    //    printf("%s\n", ANSI_CLEAR_EOL);
    printf("Power good: ");
    for (int i=0; i<POWER_GOOD_COUNT; i++) {
        printf("%s %s   ", pgood_label(i), pgood[i] ? STR_ON : STR_OFF);
    }
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_powermon(const Dev_powermon *pm)
{
    const PmState pmState = pm->pmState;
//    print_clearbox(DISPLAY_POWERMON_Y, DISPLAY_POWERMON_H);
    print_goto(DISPLAY_POWERMON_Y, 1);
    printf("Powermon state: %s", pmStateStr(pmState));
    printf("%s\n", ANSI_CLEAR_EOL);
    if (pmState == PM_STATE_INIT) {
        print_clearbox(DISPLAY_POWERMON_Y+1, DISPLAY_POWERMON_H-1);
    } else {
        print_pm_switches(pm->sw_state);
        pm_pgood_print(pm->pgood);
        printf("%s\n", ANSI_CLEAR_EOL);
    }
}

static void print_sensors(const Dev_powermon *pm)
{
    const PmState pmState = pm->pmState;
    if (pmState == PM_STATE_INIT) {
        print_clearbox(DISPLAY_SENSORS_Y, DISPLAY_SENSORS_H);
    } else {
        print_goto(DISPLAY_SENSORS_Y, 1);
        SensorStatus sensorStatus = pm_sensors_getStatus(pm);
        printf("Power supplies: %4.1f W, %4.1f W max %s\n",
               pm_get_power_w(pm),
               pm_get_power_max_w(pm),
               sensor_status_ansi_str(sensorStatus));
        monPrintValues(pm);
    }
}

static void print_thset(const Dev_thset *d)
{
    print_goto(DISPLAY_SENSORS_Y + DISPLAY_SENSORS_H - 1, 1);
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        if (d->th[i].valid)
            printf("%5.1f", d->th[i].rawTemp / 32.0);
        else
            printf(" --- ");
        printf(" ");
    }
    const SensorStatus status = dev_thset_thermStatus(d);
    printf("%s", sensor_status_ansi_str(status));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_main(const Devices *dev)
{
    print_goto(DISPLAY_MAIN_Y, 1);
//    if (getMainState() == MAIN_STATE_RUN) {
//        printf("Main state:     %s", mainStateStr(getMainState()));
        printf("%s\n", ANSI_CLEAR_EOL);
        devPrintStatus(dev);
//        printf("%s\n", ANSI_CLEAR_EOL);
//    } else {
//        print_clearbox(DISPLAY_MAIN_Y, DISPLAY_MAIN_H);
//    }
}

static void print_ttvxs_clkmux(const Dev_ttvxs_clkmux *clkmux)
{
    print_goto(DISPLAY_CLKMUX_Y, 1);
    printf("CLKMUX");
    printf(sensor_status_ansi_str(get_clkmux_sensor_status(clkmux)));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_fpga(const Dev_fpga *fpga)
{
    print_goto(DISPLAY_FPGA_Y, 1);
    printf("FPGA %s",
           fpga->initb ? "" : ANSI_RED "INIT " ANSI_CLEAR);
    if (fpga->initb && !fpga->done)
        printf(ANSI_YELLOW "loading" ANSI_CLEAR);
    if (fpga->done)
        printf("%04X", fpga->id);
    printf(sensor_status_ansi_str(get_fpga_sensor_status(fpga)));
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_pll(const Dev_ad9545 *pll)
{
    print_goto(DISPLAY_PLL_Y, 1);
    printf("PLL AD9545:      %s %s",
           dev_ad9545_state_str(pll->fsm_state),
           sensor_status_ansi_str(get_pll_sensor_status(pll)));
    printf("%s\n", ANSI_CLEAR_EOL);
    ad9545_brief_status(&pll->status);
}

static void print_auxpll(const Dev_auxpll *pll)
{
    print_goto(DISPLAY_AUXPLL_Y, 1);
    auxpllPrint(pll);
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
        printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_log_messages(void)
{
    //    print_clearbox(DISPLAY_LOG_Y, DISPLAY_LOG_H);
        print_goto(DISPLAY_LOG_Y, 1);
        print_log_lines(DISPLAY_LOG_H);
}

static void display_log(void)
{
    print_goto(2, 1);
    printf("Log messages\n" ANSI_CLEAR_EOL);
    print_goto(3, 1);
    print_log_lines(DISPLAY_HEIGHT - 3);
}

static int old_enable_stats_display = 0;

static void display_summary(const Devices * dev)
{
    print_powermon(&dev->pm);
    if (enable_stats_display) {
        print_sensors(&dev->pm);
    }
    print_thset(&dev->thset);
    print_main(dev);
    print_ttvxs_clkmux(&dev->clkmux);
    print_fpga(&dev->fpga);
    print_pll(&dev->pll);
    print_auxpll(&dev->auxpll);
    print_log_messages();
}

static void display_boards(const Devices * dev)
{
    print_goto(2, 1);
    printf("Boards\n" ANSI_CLEAR_EOL);
    printf(" # exp  merr serr BMC  FPGA     up   all power therm  misc  fpga   pll" ANSI_CLEAR_EOL "\n");
    int line = 0;
    for (uint32_t i=0; i<VXSIIC_SLOTS; i++) {
        const vxsiic_slot_status_t *status = &dev->vxsiic.status.slot[i];
        const char *label = vxsiic_map_slot_to_label[i];
        if (0 == status->present)
            printf("%2s" ANSI_CLEAR_EOL "\n", label);
        else
            printf("%2s  %s%s %4lu %4lu %2u.%-2u  %02lX %7lu  %s  %s  %s  %s  %s  %s" ANSI_CLEAR_EOL "\n",
                   label,
                   (status->ioexp & VXSIIC_PP_IOEXP_BIT_PGOOD) ? "P" : ".",
                   (status->ioexp & VXSIIC_PP_IOEXP_BIT_DONE) ? "D" : ".",
                   status->iic_master_stats.errors,
                   status->iic_stats.errors,
                   (uint16_t)(status->bmc_ver >> 16),
                   (uint16_t)status->bmc_ver,
                   status->module_id & 0xFF,
                   status->uptime,
                   sensor_status_str(status->enc_status.b.system),
                   sensor_status_str(status->enc_status.b.pm),
                   sensor_status_str(status->enc_status.b.therm),
                   sensor_status_str(status->enc_status.b.misc),
                   sensor_status_str(status->enc_status.b.fpga),
                   sensor_status_str(status->enc_status.b.pll)
                   );
        line++;
    }
    print_clearbox(4+line, VXSIIC_SLOTS-line);
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
    dev_ad9545_verbose_status(&dev->pll);
}

static void display_auxpll_detail(const Devices * dev)
{
    print_clearbox(DISPLAY_AUXPLL_DETAIL_Y, DISPLAY_AUXPLL_DETAIL_H);
    print_goto(DISPLAY_AUXPLL_DETAIL_Y, 1);
    auxpllPrintStatus(&dev->auxpll);
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

    const Devices * d = getDevices();
    int need_clear_screen =
            display_mode == DISPLAY_NONE
            || display_mode == DISPLAY_LOG
            || display_mode == DISPLAY_TASKS
            || display_mode == DISPLAY_SFP_DETAIL;
//            || display_mode == DISPLAY_BOARDS;
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
    case DISPLAY_PLL_DETAIL:
        display_pll_detail(d);
        display_auxpll_detail(d);
        break;
//    case DISPLAY_BOARDS:
//        display_boards(d);
//        break;
    case DISPLAY_SFP_DETAIL:
        sfpPrintStatus(&d->sfpiic);
        break;
    case DISPLAY_TASKS:
        display_tasks();
        break;
    case DISPLAY_NONE:
        break;
    default:
        break;
    }
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
