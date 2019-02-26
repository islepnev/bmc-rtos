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

#include "cpu_cycle.h"
#include "fpga_spi_hal.h"
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

#include "app_shared_data.h"
#include "app_task_powermon.h"
#include "app_task_main.h"

static const int displayThreadStackSize = 1000;

static uint32_t displayUpdateCount = 0;
static const uint32_t displayUpdateInterval = 1000;

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

static void dev_thset_print(const Dev_thset *d)
{
    printf("Temp: ");
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        print_adt7301_value(d->th[i].rawTemp);
        printf(" ");
    }
    const SensorStatus status = dev_thset_thermStatus(d);
    printf("%s\n", sensorStatusStr(status));
}

void dev_print_thermometers(const Devices *dev)
{
    if (pm_sensor_isValid(&dev->pm.sensors[SENSOR_VME_5V])) { // 5V
        dev_thset_print(&dev->thset);
    } else {
        printf("Temp: no power\n");
    }
}

static void update_display(const Devices * dev)
{
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);
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
    printf("\n");
    const PmState pmState = getPmState();
    printf("Powermon state: %s\n", pmStateStr(pmState));
    print_pm_switches(dev->pm.sw);
    pm_pgood_print(dev->pm);
    if (pmState == PM_STATE_RAMP || pmState == PM_STATE_RUN) {
        SensorStatus sensors = pm_sensors_getStatus(&dev->pm);
        printf("System power supplies: %s\n", sensorStatusStr(sensors));
        printf("\n");
        monPrintValues(&dev->pm);
        dev_print_thermometers(dev);
    }
    if (getMainState() == MAIN_STATE_RUN) {
        printf("\n");
        printf("Main state:     %s\n", mainStateStr(getMainState()));
        devPrintStatus(dev);
    }
    printf("\n");
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

osThreadDef(displayThread, displayTask, osPriorityIdle,      1, displayThreadStackSize);

void create_task_display(void)
{
    osThreadId displayThreadId = osThreadCreate(osThread (displayThread), NULL);
    if (displayThreadId == NULL) {
        printf("Failed to create Display thread\n");
    }
}
