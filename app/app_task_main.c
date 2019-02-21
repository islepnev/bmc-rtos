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

#include "FreeRTOS.h"
#include "task.h"
//#include "semphr.h"

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

uint32_t mainloopCount = 0;

uint32_t heartbeatCount = 0;
const uint32_t heartbeatInterval = 10;
uint32_t heartbeatUpdateTick = 999999; // run in first loop

uint32_t displayUpdateCount = 0;
const uint32_t displayUpdateInterval = 1000;
uint32_t displayUpdateTick = 999999;

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

static void task_display(void)
{
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);

    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX\n",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("Uptime: %-8ld Mainloop %-8ld Heartbeat %-6ld DisplayUpdate %-6ld\n",
           HAL_GetTick() / getTickFreqHz(), mainloopCount, heartbeatCount, displayUpdateCount);
    print_pm_switches(dev.pm.sw);
    int systemPowerState = getPowerMonState(dev.pm);
    printf("System power supplies: %s\n", systemPowerState ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    pm_pgood_print(dev.pm);
    dev_print_thermometers(dev);
    devPrintStatus(dev);
    monPrintValues(dev.pm);
    fflush(stdout);

}

static void prvAppMainTask( void *pvParameters )
{
    (void) pvParameters;
    while (1)
    {
        task_main();
        const uint32_t tick = HAL_GetTick();
        if (tick - heartbeatUpdateTick > heartbeatInterval) {
            heartbeatUpdateTick = tick;
            heartbeatCount++;
            task_heartbeat();
        };
        if (tick - displayUpdateTick > displayUpdateInterval) {
            displayUpdateTick = tick;
            displayUpdateCount++;
            task_display();
        }
    }
}

void create_task_main(int priority)
{
    xTaskCreate( prvAppMainTask,
                 "Main", // thread name, debug only
                 4 * configMINIMAL_STACK_SIZE,    // stack size
                 NULL,   // *pvParameters
                 priority,
                 NULL ); // task handle
}
