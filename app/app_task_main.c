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

#include "app_task_main.h"

#include <stdint.h>

#include "cmsis_os.h"

#include "fpga_spi_hal.h"
#include "adt7301_spi_hal.h"
#include "pca9548_i2c_hal.h"
#include "ina226_i2c_hal.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "ftoa.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "devices.h"
#include "dev_types.h"
#include "dev_mcu.h"
#include "dev_leds.h"
#include "cpu_cycle.h"

#include "app_shared_data.h"

static const int mainThreadStackSize = 1000;
static const int displayThreadStackSize = 1000;

static uint32_t mainloopCount = 0;

static uint32_t displayUpdateCount = 0;
static const uint32_t displayUpdateInterval = 300;

typedef enum {
    STATE_INIT,
    STATE_STANDBY, // bus power switches off
    STATE_RAMP_5V, // voltage ramp up
    STATE_RAMP,    // voltage ramp up
    STATE_DETECT,
    STATE_RUN,
    STATE_PWRFAIL,
    STATE_ERROR
} MainState;

const char *mainStateStr(MainState mainState)
{
    switch(mainState) {
    case STATE_INIT:    return "INIT";
    case STATE_STANDBY: return "STANDBY";
    case STATE_RAMP_5V: return ANSI_YELLOW "RAMP_5V" ANSI_CLEAR;
    case STATE_RAMP:    return ANSI_YELLOW "RAMP"    ANSI_CLEAR;
    case STATE_DETECT:  return ANSI_YELLOW "DETECT"  ANSI_CLEAR;
    case STATE_RUN:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case STATE_PWRFAIL: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    default: return "?";
    }
}

static MainState mainState = STATE_INIT;
uint32_t stateStartTick = 0;
uint32_t stateTicks(void)
{
    return HAL_GetTick() - stateStartTick;
}

static void task_main (void)
{
    mainloopCount++;
    const MainState oldState = mainState;
    led_toggle(LED_YELLOW);
    int pgood = dev_readPgood(&dev.pm);
    int power_5v_ok = (pgood
                    && (dev.pm.monState == MON_STATE_READ)
                    && (getSensorIsValid_5V(&dev.pm)));
    int power_all_ok = (pgood
                    && (dev.pm.monState == MON_STATE_READ)
                    && (getPowerMonState(&dev.pm)));
;
    switch (mainState) {
    case STATE_INIT:
        struct_powermon_init(&dev.pm);
        struct_Devices_init(&dev);
        mainState = STATE_STANDBY;
        break;
    case STATE_STANDBY:
        if (stateTicks() > 1000) {
            dev_switchPower(&dev.pm, SWITCH_ON);
            mainState = STATE_RAMP_5V;
        }
        break;
    case STATE_RAMP_5V:
        if (power_5v_ok) {
            mainState = STATE_RAMP;
        }
        if (stateTicks() > 1000) {
            printf("RAMP_5V timeout\n");
            mainState = STATE_PWRFAIL;
        }
        break;
    case STATE_RAMP:
        if (pgood && dev.pm.monState == MON_STATE_READ) {
            if (getPowerMonState(&dev.pm)) {
                mainState = STATE_DETECT;
            }
        }
        if (stateTicks() > 1000) {
            printf("RAMP timeout\n");
            mainState = STATE_PWRFAIL;
        }
        break;
    case STATE_DETECT:
        if (!power_all_ok) {
                printf("Power failure in DETECT\n");
                mainState = STATE_PWRFAIL;
                break;
        }
        if (getDeviceStatus(&dev) == DEVICE_NORMAL)
            mainState = STATE_RUN;
        if (stateTicks() > 2000) {
            printf("DETECT timeout\n");
            mainState = STATE_ERROR;
        }
        break;
    case STATE_RUN:
        if (!power_all_ok) {
                printf("Power failure in RUN\n");
                mainState = STATE_PWRFAIL;
                break;
        }
        if (dev.pm.monState != MON_STATE_READ) {
                printf("Error in STATE_RUN\n");
                mainState = STATE_ERROR;
                break;
        }
        break;
    case STATE_PWRFAIL:
        dev_switchPower(&dev.pm, SWITCH_OFF);
        dev_readPgood(&dev.pm);
        if (stateTicks() > 2000) {
            mainState = STATE_STANDBY;
        }
        break;
    case STATE_ERROR:
        dev_switchPower(&dev.pm, SWITCH_OFF);
        if (stateTicks() > 2000) {
            mainState = STATE_STANDBY;
        }
        break;
    }

    dev_led_set(&dev.leds, LED_RED, mainState == STATE_PWRFAIL || mainState == STATE_ERROR);

    if (mainState == STATE_RAMP_5V
            || mainState == STATE_RAMP
            || mainState == STATE_DETECT
            || mainState == STATE_RUN) {
        runMon(&dev.pm);
    } else {
        struct_powermon_init(&dev.pm);
    }
    if (mainState == STATE_DETECT) {
        devReset(&dev);
    }
    if (mainState == STATE_DETECT || mainState == STATE_RUN) {
        devDetect(&dev);
    } else {
        struct_Devices_init(&dev);
    }
    if ((mainState == STATE_DETECT || mainState == STATE_RUN)
            && getSensorIsValid_5V(&dev.pm)) {
        dev_read_thermometers(&dev);
    } else {
        struct_thset_init(&dev.thset);
    }

    if (oldState != mainState) {
        stateStartTick = HAL_GetTick();
    }

//    dev_led_set(&dev.leds, LED_YELLOW, devStatus != DEVICE_NORMAL);

//    int systemPowerState = getPowerMonState(&dev.pm);
//    dev_led_set(&dev.leds, LED_RED, !systemPowerState);
}

void unused1(void)
{
    enable_cpu_cycle_counter();
    const uint32_t startTick = cpu_getCycles();
    uint32_t time1 = cpu_getCycles();
    uint32_t time2 = cpu_getCycles();
    uint32_t time3 = cpu_getCycles();
    uint32_t time4 = cpu_getCycles();
    time1 -= startTick;
    time2 -= startTick;
    time3 -= startTick;
    time4 -= startTick;
    printf(ANSI_NONE "%-8ld %-8ld %-8ld %-8ld \n",
           (uint32_t)((float)time1 * 1e3f / HAL_RCC_GetHCLKFreq()),
           (uint32_t)((float)time2 * 1e3f / HAL_RCC_GetHCLKFreq()),
           (uint32_t)((float)time3 * 1e3f / HAL_RCC_GetHCLKFreq()),
           (uint32_t)((float)time4 * 1e3f / HAL_RCC_GetHCLKFreq())
           );
}

static void update_display(const Devices * dev)
{
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);

    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX\n",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("Uptime: %-8ld Mainloop %-8ld DisplayUpdate %-6ld\n",
           HAL_GetTick() / getTickFreqHz(), mainloopCount, displayUpdateCount);
    printf("Main state: %s\n", mainStateStr(mainState));
    print_pm_switches(dev->pm.sw);
    int systemPowerState = getPowerMonState(&dev->pm);
    printf("System power supplies: %s\n", systemPowerState ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    pm_pgood_print(dev->pm);
    dev_print_thermometers(dev);
    devPrintStatus(dev);
    monPrintValues(&dev->pm);
    fflush(stdout);
    displayUpdateCount++;
}

static void displayTask(void const *arg)
{
    (void) arg;
    while(1) {
        update_display(&dev);
        osDelay(displayUpdateInterval);
    }
}

static void prvAppMainTask( void const *arg)
{
    while (1)
    {
        task_main();
//        osThreadYield();
        osDelay(5);
    }
}

osThreadDef(mainThread, prvAppMainTask, osPriorityAboveNormal,      1, mainThreadStackSize);
osThreadDef(displayThread, displayTask, osPriorityIdle,      1, displayThreadStackSize);

void create_task_main(void)
{
    osThreadId mainThreadId = osThreadCreate(osThread (mainThread), NULL);
    if (mainThreadId == NULL) {
        printf("Failed to create Main thread\n");
    }
    osThreadId displayThreadId = osThreadCreate(osThread (displayThread), NULL);
    if (displayThreadId == NULL) {
        printf("Failed to create Display thread\n");
    }
}
