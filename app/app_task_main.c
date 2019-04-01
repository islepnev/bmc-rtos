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

#include "dev_fpga.h"
//#include "adt7301_spi_hal.h"
//#include "pca9548_i2c_hal.h"
//#include "ina226_i2c_hal.h"
#include "dev_types.h"
//#include "dev_eeprom.h"
#include "dev_powermon.h"
//#include "ansi_escape_codes.h"
//#include "display.h"
//#include "dev_mcu.h"
//#include "dev_leds.h"
#include "debug_helpers.h"
#include "logbuffer.h"
#include "devices.h"
#include "version.h"

#include "app_shared_data.h"
#include "app_task_powermon.h"
#include "app_tasks.h"

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

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->vxsiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->eeprom_config.present != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getFpgaStatus(const Dev_fpga *d)
{
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->id != FPGA_DEVICE_ID)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getPllStatus(const Dev_ad9545 *d)
{
    if (d->fsm_state == PLL_STATE_ERROR || d->fsm_state == PLL_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.stable ||
            !d->status.sysclk.b.pll0_locked ||
            !d->status.sysclk.b.pll1_locked
            )
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getSystemStatus(const Devices *dev)
{
    const SensorStatus powermonStatus = getPowermonStatus(&dev->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&dev->thset);
    const SensorStatus miscStatus = getMiscStatus(dev);
    const SensorStatus fpgaStatus = getFpgaStatus(&dev->fpga);
    const SensorStatus pllStatus = getPllStatus(&dev->pll);
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (powermonStatus > systemStatus)
        systemStatus = powermonStatus;
    if (temperatureStatus > systemStatus)
        systemStatus = temperatureStatus;
    if (miscStatus > systemStatus)
        systemStatus = miscStatus;
    if (fpgaStatus > systemStatus)
        systemStatus = fpgaStatus;
    if (pllStatus > systemStatus)
        systemStatus = pllStatus;
    return systemStatus;
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
            log_printf(LOG_ERR, "DETECT timeout");
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

    if (mainState == MAIN_STATE_DETECT || mainState == MAIN_STATE_RUN) {
        devDetect(&dev);
        fpgaWriteBmcVersion();
        fpgaWriteBmcTemperature(&dev.thset);
        fpgaWritePllStatus(&dev.pll);
    } else {
//        struct_Devices_init(&dev);
    }
    if (mainState == MAIN_STATE_RUN) {
        devRun(&dev);
    }
    enable_pll_run = (pmState == PM_STATE_RUN);
    if (oldState != mainState) {
        stateStartTick = HAL_GetTick();
    }

    const SensorStatus systemStatus = getSystemStatus(&dev);
    dev_led_set(&dev.leds, LED_RED, systemStatus >= SENSOR_CRITICAL);
    dev_led_set(&dev.leds, LED_YELLOW, systemStatus >= SENSOR_WARNING);
    dev_led_set(&dev.leds, LED_GREEN, systemStatus == SENSOR_NORMAL);
}

static void prvAppMainTask( void const *arg)
{
    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
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
        debug_printf("Failed to create main thread\n");
    }
}
