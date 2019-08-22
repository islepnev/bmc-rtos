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
#include <stdio.h>

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

#include "dev_types.h"
#include "dev_powermon.h"
//#include "dev_leds.h"
#include "debug_helpers.h"
#include "logbuffer.h"
#include "devices.h"
#include "version.h"
#include "i2c.h"

#include "app_shared_data.h"
#include "app_task_powermon.h"
#include "app_tasks.h"
#include "system_status.h"

static const uint32_t MAIN_DETECT_TIMEOUT_TICKS = 5000;

enum { mainThreadStackSize = threadStackSize };
static const uint32_t mainTaskLoopDelay = 10;

static uint32_t mainloopCount = 0;

uint32_t getMainLoopCount(void)
{
    return mainloopCount;
}

static MainState mainState = MAIN_STATE_INIT;

MainState getMainState(void)
{
    return mainState;
}

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return HAL_GetTick() - stateStartTick;
}

static void task_main (void)
{
    mainloopCount++;
    const MainState oldState = mainState;
    const PmState pmState = getPmState();
//    led_toggle(LED_YELLOW);

    switch (mainState) {
    case MAIN_STATE_INIT:
        if (pmState == PM_STATE_RUN) {
            struct_Devices_init(&dev);
            mainState = MAIN_STATE_DETECT;
        }
        break;
    case MAIN_STATE_DETECT:
        if (pmState != PM_STATE_RUN) {
            mainState = MAIN_STATE_INIT;
        }
        if (getDeviceStatus(&dev) == DEVICE_NORMAL)
            mainState = MAIN_STATE_RUN;
        if (stateTicks() > MAIN_DETECT_TIMEOUT_TICKS) {
            log_put(LOG_ERR, "DETECT timeout");
            mainState = MAIN_STATE_ERROR;
        }
        break;
    case MAIN_STATE_RUN:
        if (pmState != PM_STATE_RUN) {
            mainState = MAIN_STATE_INIT;
        }
        break;
    case MAIN_STATE_ERROR:
        if (stateTicks() > 2000) {
            mainState = MAIN_STATE_INIT;
        }
        break;
    }

    if (mainState == MAIN_STATE_DETECT) {
        devDetect(&dev);
    }
    if (mainState == MAIN_STATE_RUN) {
        devRun(&dev);
    }
    enable_pll_run = (pmState == PM_STATE_RUN);
    if (oldState != mainState) {
        stateStartTick = HAL_GetTick();
    }

    const SensorStatus systemStatus = getSystemStatus(&dev);
    const SensorStatus vxsiicStatus = pollVxsiicStatus(&dev);
    SensorStatus showStatus = systemStatus;
    if (vxsiicStatus > showStatus)
        showStatus = vxsiicStatus;

    dev_led_set(&dev.leds, LED_RED,    showStatus >= SENSOR_CRITICAL);
    dev_led_set(&dev.leds, LED_YELLOW, showStatus >= SENSOR_WARNING);
    dev_led_set(&dev.leds, LED_GREEN,  showStatus == SENSOR_NORMAL);
}

static void prvAppMainTask( void const *arg)
{
//    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    while (1)
    {
        task_main();
        osDelay(mainTaskLoopDelay);
    }
}

osThreadDef(main, prvAppMainTask, osPriorityNormal,      1, mainThreadStackSize);

void create_task_main(void)
{
    osThreadId mainThreadId = osThreadCreate(osThread (main), NULL);
    if (mainThreadId == NULL) {
        debug_print("Failed to create main thread\n");
    }
}
