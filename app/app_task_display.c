//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "app_task_display.h"

#include <stdint.h>

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

#include "fpga_spi_hal.h"
#include "ad9545_util.h"
#include "adt7301_spi_hal.h"
#include "pca9548_i2c_hal.h"
#include "ina226_i2c_hal.h"
#include "dev_types.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "ftoa.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "dev_mcu.h"
#include "dev_leds.h"
#include "devices.h"
#include "version.h"
#include "logbuffer.h"

#include "app_shared_data.h"
#include "app_task_powermon.h"
#include "app_task_main.h"
#include "app_tasks.h"

osThreadId displayThreadId = NULL;

enum { displayThreadStackSize = threadStackSize };

static uint32_t displayUpdateCount = 0;
static const uint32_t displayTaskLoopDelay = 500;

static const char *pmStateStr(PmState state)
{
    switch(state) {
    case PM_STATE_INIT:    return "INIT";
    case PM_STATE_STANDBY: return "STANDBY";
    case PM_STATE_RAMP_5V: return ANSI_YELLOW "RAMP_5V" ANSI_CLEAR;
    case PM_STATE_RAMP:    return ANSI_YELLOW "RAMP"    ANSI_CLEAR;
    case PM_STATE_RUN:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case PM_STATE_PWRFAIL: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case PM_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    default: return "?";
    }
}

static const char *sensorStatusStr(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return STR_RESULT_WARNING;
    case SENSOR_NORMAL:   return STR_RESULT_NORMAL;
    case SENSOR_WARNING:  return STR_RESULT_WARNING;
    case SENSOR_CRITICAL: return STR_RESULT_CRIT;
    default: return STR_RESULT_FAIL;
    }
}

static const char *mainStateStr(MainState state)
{
    switch(state) {
    case MAIN_STATE_INIT:    return "INIT";
    case MAIN_STATE_DETECT:  return ANSI_YELLOW "DETECT"  ANSI_CLEAR;
    case MAIN_STATE_RUN:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case MAIN_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    default: return "?";
    }
}

static const char *pllStateStr(PllState state)
{
    switch(state) {
    case PLL_STATE_RESET:    return "RESET";
    case PLL_STATE_SETUP_SYSCLK:    return "SETUP_SYSCLK";
    case PLL_STATE_SYSCLK_WAITLOCK: return ANSI_YELLOW  "SYSCLK_WAITLOCK"     ANSI_CLEAR;
    case PLL_STATE_SETUP:     return ANSI_GREEN  "SETUP"     ANSI_CLEAR;
    case PLL_STATE_RUN:   return ANSI_GREEN    "RUN"   ANSI_CLEAR;
    case PLL_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case PLL_STATE_FATAL:   return ANSI_RED    "FATAL"   ANSI_CLEAR;
    default: return "?";
    }
}

static void dev_thset_print(const Dev_thset *d)
{
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        if (d->th[i].valid)
            print_adt7301_value(d->th[i].rawTemp);
        else
            printf(" --- ");
        printf(" ");
    }
    const SensorStatus status = dev_thset_thermStatus(d);
    printf("%s", sensorStatusStr(status));
}

void dev_print_thermometers(const Devices *dev)
{
    if (getSensorIsValid_3V3(&dev->pm)) {
        dev_thset_print(&dev->thset);
    } else {
        printf("Temp: no power");
    }
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void print_pm_switches(const pm_switches sw)
{
    printf("Switch 5V %s   3.3V %s   1.5V %s   1.0V %s",
           sw.switch_5v  ? STR_ON : STR_OFF,
           sw.switch_3v3 ? STR_ON : STR_OFF,
           sw.switch_1v5 ? STR_ON : STR_OFF,
           sw.switch_1v0 ? STR_ON : STR_OFF);
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void pm_pgood_print(const Dev_powermon pm)
{
//    printf("Live insert: %s", pm.vmePresent ? STR_RESULT_ON : STR_RESULT_OFF);
//    printf("%s\n", ANSI_CLEAR_EOL);
    printf("Intermediate 1.5V: %s", pm.ltm_pgood ? STR_RESULT_NORMAL : pm.sw.switch_1v5 ? STR_RESULT_CRIT : STR_RESULT_OFF);
    printf("%s\n", ANSI_CLEAR_EOL);
    printf("FPGA Core 1.0V:    %s", pm.fpga_core_pgood ? STR_RESULT_NORMAL : pm.sw.switch_1v0 ? STR_RESULT_CRIT : STR_RESULT_OFF);
    printf("%s\n", ANSI_CLEAR_EOL);
}

static void pm_sensor_print(const pm_sensor *d, int isOn)
{
    printf("%6s: ", d->label);
    if (isOn && d->deviceStatus != DEVICE_UNKNOWN) {
        if (d->deviceStatus == DEVICE_FAIL) {
            printf("FAIL");
        }
        const int fractdigits = 3;
        char str1[10], str3[10];
        //    char str2[10];
        ftoa(d->busVoltage, str1, fractdigits);
        SensorStatus status = pm_sensor_status(d);
        const char *color = "";
        switch (status) {
        case SENSOR_UNKNOWN: color = ANSI_YELLOW; break;
        case SENSOR_NORMAL: color = ANSI_GREEN; break;
        case SENSOR_WARNING: color = ANSI_YELLOW; break;
        case SENSOR_CRITICAL: color = ANSI_RED; break;
        }
        printf("%s%8s%s", color, str1, ANSI_CLEAR);
        if (d->shuntVal > SENSOR_MINIMAL_SHUNT_VAL) {
            ftoa(d->current, str3, fractdigits);
            printf(" %8s", str3);
            ftoa(d->currentMax, str3, fractdigits);
            printf(" %8s", str3);
        } else {
            printf("         ");
        }
        printf(" %s   %ld", isOn ? (pm_sensor_isValid(d) ? STR_RESULT_NORMAL : STR_RESULT_FAIL) : STR_RESULT_OFF,
               pm_sensor_get_sensorStatus_Duration(d) / getTickFreqHz());
    } else {
        printf(ANSI_CLEAR_EOL);
    }
}

static const char *monStateStr(MonState monState)
{
    switch(monState) {
    case MON_STATE_INIT: return "INIT";
    case MON_STATE_DETECT: return "DETECT";
    case MON_STATE_READ: return "READ";
    case MON_STATE_ERROR: return "ERROR";
    default: return "?";
    }
}

void monPrintValues(const Dev_powermon *d)
{
    printf("Sensors state:  %s %s",
           monStateStr(d->monState), d->monErrors ? STR_RESULT_FAIL : STR_RESULT_NORMAL);
    if (d->monErrors)
        printf("     %d errors", d->monErrors);
    printf("%s\n", ANSI_CLEAR_EOL);
//    if (d->monState == MON_STATE_READ)
    {
        for (int i=0; i<POWERMON_SENSORS; i++) {
//            uint16_t deviceAddr = monAddr[i];
//            printMonValue(deviceAddr, monValuesBus[i], monValuesShunt[i], monShuntVal(deviceAddr));
            pm_sensor_print(&d->sensors[i], monIsOn(d->sw, i));
            printf("%s\n", ANSI_CLEAR_EOL);
        }
    }
}

void pllPrint(const Dev_ad9545 *d)
{
    printf("PLL state:      %s", pllStateStr(d->fsm_state));
    printf("%s\n", ANSI_CLEAR_EOL);
    if (d->fsm_state == PLL_STATE_RUN) {
        printf("Ref A:");
        pllPrintRefStatusBits(d->status.ref[REFA]);
        printf("%s\n", ANSI_CLEAR_EOL);
        printf("Ref B:");
        pllPrintRefStatusBits(d->status.ref[REFB]);
        printf("%s\n", ANSI_CLEAR_EOL);
        for (int channel=0; channel<DPLL_COUNT; channel++) {
            int64_t ppb0 = pll_ftw_rel_ppb(d, channel);
            const char *ref0str = "";
            ProfileRefSource_TypeDef ref0 = pll_get_current_ref(d, channel);
            if (ref0 != PROFILE_REF_SOURCE_INVALID)
                ref0str = pllProfileRefSourceStr(ref0);
            printf("PLL%d: %s ref %-5s %lld ppb",
                   channel,
                   d->status.sysclk.b.pll0_locked ? ANSI_GREEN "LOCKED  " ANSI_CLEAR: ANSI_RED "UNLOCKED" ANSI_CLEAR,
                   ref0str,
                   (int64_t)ppb0
                   );
            printf("%s\n", ANSI_CLEAR_EOL);
        }
    } else {
        for (int i=0; i<4; i++)
            printf("%s\n", ANSI_CLEAR_EOL);
    }
}

static void print_log_entry(uint32_t index)
{
    LogEntry ent;
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

//static uint32_t log_rptr = 0;
//static uint32_t log_n = 0;

#define DISPLAY_POWERMON_Y 2
#define DISPLAY_POWERMON_H 4
#define DISPLAY_SENSORS_Y (0 + DISPLAY_POWERMON_Y + DISPLAY_POWERMON_H)
#define DISPLAY_SENSORS_H 16
#define DISPLAY_MAIN_Y (0 + DISPLAY_SENSORS_Y + DISPLAY_SENSORS_H)
#define DISPLAY_MAIN_H 5
#define DISPLAY_PLL_Y (0 + DISPLAY_MAIN_Y + DISPLAY_MAIN_H)
#define DISPLAY_PLL_H 5
#define DISPLAY_LOG_Y (1 + DISPLAY_PLL_Y + DISPLAY_PLL_H)
#define DISPLAY_LOG_H (LOG_BUF_SIZE)

static void print_goto(int line, int col)
{
    printf("\x1B[%d;%dH", line, col);
}

static void print_clearbox(int line1, int height)
{
    for(int i=line1; i<line1+height; i++)
        printf("\x1B[%d;H\x1B[K", i);
}

static void print_log_messages(void)
{
//    print_clearbox(DISPLAY_LOG_Y, DISPLAY_LOG_H);
    print_goto(DISPLAY_LOG_Y, 1);
    const uint32_t log_count = log_get_count();
    const uint32_t log_wptr = log_get_wptr();
    if (log_count < LOG_BUF_SIZE) {
        for (uint32_t i=0; i<log_wptr; i++)
            print_log_entry(i);
        for (uint32_t i=log_wptr; i<LOG_BUF_SIZE; i++)
            printf("%s\n", ANSI_CLEAR_EOL);
    } else {
        for (uint32_t i=log_wptr; i<LOG_BUF_SIZE; i++)
            print_log_entry(i);
        for (uint32_t i=0; i<log_wptr; i++)
            print_log_entry(i);
    }
//    char str[16];
//    snprintf(str, 16, "%ld", log_n++);
//    log_put(1, str);
}

static char statsBuffer[1000];

static void update_display(const Devices * dev)
{
    //    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);
    printf(ANSI_GOHOME ANSI_CLEAR);
    printf(CSI"?25l"); // hide cursor
    // Title
    uint32_t uptimeSec = osKernelSysTick() / osKernelSysTickFrequency;
    printf("%s%s v%s%s", ANSI_BOLD ANSI_BGR_BLUE ANSI_GRAY, APP_NAME_STR, VERSION_STR, ANSI_CLEAR ANSI_BGR_BLUE);
    printf("     Uptime: %-8ld", uptimeSec);
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
    if (0) printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX\n",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    if (0) printf("PmLoop %-8ld Mainloop %-8ld DisplayUpdate %-6ld\n",
           getPmLoopCount(),
           getMainLoopCount(),
           displayUpdateCount);
//    printf("\n");
    // Powermon
    const PmState pmState = getPmState();
//    print_clearbox(DISPLAY_POWERMON_Y, DISPLAY_POWERMON_H);
    print_goto(DISPLAY_POWERMON_Y, 1);
    printf("Powermon state: %s", pmStateStr(pmState));
    printf("%s\n", ANSI_CLEAR_EOL);
    print_pm_switches(dev->pm.sw);
    pm_pgood_print(dev->pm);
    printf("%s\n", ANSI_CLEAR_EOL);
    // Sensors
//    print_clearbox(DISPLAY_SENSORS_Y, DISPLAY_SENSORS_H);
    print_goto(DISPLAY_SENSORS_Y, 1);
//    if (pmState == PM_STATE_RAMP || pmState == PM_STATE_RUN) {
        SensorStatus sensors = pm_sensors_getStatus(&dev->pm);
        printf("System power supplies: %s\n", sensorStatusStr(sensors));
//        printf("\n");
        monPrintValues(&dev->pm);
        dev_print_thermometers(dev);
//    }
//    printf("%s\n", ANSI_CLEAR_EOL);
//    print_clearbox(DISPLAY_MAIN_Y, DISPLAY_MAIN_H);
    print_goto(DISPLAY_MAIN_Y, 1);
    if (getMainState() == MAIN_STATE_RUN) {
        printf("Main state:     %s", mainStateStr(getMainState()));
        printf("%s\n", ANSI_CLEAR_EOL);
        devPrintStatus(dev);
//        printf("%s\n", ANSI_CLEAR_EOL);
    } else {
        print_clearbox(DISPLAY_MAIN_Y, DISPLAY_MAIN_H);
    }
    print_goto(DISPLAY_PLL_Y, 1);
    pllPrint(&dev->pll);
    print_log_messages();

    vTaskGetRunTimeStats((char *)&statsBuffer);
    printf("%s", statsBuffer);
    printf("%s\n", ANSI_CLEAR_EOL);

    printf(CSI"?25h"); // show cursor
    printf("%s", ANSI_CLEAR_EOL);
    fflush(stdout);
    displayUpdateCount++;
}

static void read_keys(void)
{
    int ch = getchar();
    if (ch == EOF)
        return;
    switch (ch) {
        case ' ':
//        osSignalSet(displayThreadId, SIGNAL_REFRESH_DISPLAY);
        break;
    }
}

static void displayTask(void const *arg)
{
    (void) arg;
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);
    while(1) {
        update_display(&dev);
//        read_keys();
//        osSignalWait(SIGNAL_REFRESH_DISPLAY, displayTaskLoopDelay);
        osDelay(displayTaskLoopDelay);
    }
}

osThreadDef(displayThread, displayTask, osPriorityIdle,      1, displayThreadStackSize);

void create_task_display(void)
{
    displayThreadId = osThreadCreate(osThread (displayThread), NULL);
    if (displayThreadId == NULL) {
        printf("Failed to create Display thread\n");
    }
}
