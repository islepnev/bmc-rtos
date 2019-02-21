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

#include "app_shared_data.h"

static const int mainThreadStackSize = 1000;
static const int displayThreadStackSize = 1000;

static uint32_t mainloopCount = 0;

static uint32_t heartbeatCount = 0;
static const uint32_t heartbeatInterval = 10;
static uint32_t heartbeatUpdateTick = 999999; // run in first loop

static uint32_t displayUpdateCount = 0;
static const uint32_t displayUpdateInterval = 1000;
static uint32_t displayUpdateTick = 999999;

static void task_main (void)
{
    mainloopCount++;
    // Switch ON
    //      dev_switchPower(&dev, SWITCH_OFF);
    dev_switchPower(&dev, SWITCH_ON);
    //      HAL_Delay(1500);

    //      struct_Devices_init(&dev);
    DeviceStatus devStatus = devDetect(&dev);
//    dev_led_set(&dev.leds, LED_YELLOW, devStatus != DEVICE_NORMAL);

    dev_read_thermometers(&dev);

    runMon(&dev.pm);
    int systemPowerState = getPowerMonState(dev.pm);
    dev_led_set(&dev.leds, LED_RED, !systemPowerState);
}

static void task_heartbeat(void)
{
    dev_leds_toggle(&dev.leds, LED_YELLOW);
}

static void update_display(const Devices * dev)
{
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);

    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX\n",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("Uptime: %-8ld Mainloop %-8ld Heartbeat %-6ld DisplayUpdate %-6ld\n",
           HAL_GetTick() / getTickFreqHz(), mainloopCount, heartbeatCount, displayUpdateCount);
    print_pm_switches(dev->pm.sw);
    int systemPowerState = getPowerMonState(dev->pm);
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
        osDelay(displayUpdateInterval);
        update_display(&dev);
    }
}

static void prvAppMainTask( void const *arg)
{
    while (1)
    {
        task_main();
        const uint32_t tick = HAL_GetTick();
        if (tick - heartbeatUpdateTick > heartbeatInterval) {
            heartbeatUpdateTick = tick;
            heartbeatCount++;
            task_heartbeat();
        };
//        if (tick - displayUpdateTick > displayUpdateInterval) {
//            displayUpdateTick = tick;
//            update_display(dev);
//        }
        osDelay(10);
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
