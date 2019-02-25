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

#include "app_task_powermon_impl.h"

#include <stdint.h>

#include "cmsis_os.h"

#include "app_task_powermon.h"
#include "adt7301_spi_hal.h"
#include "pca9548_i2c_hal.h"
#include "ina226_i2c_hal.h"
#include "dev_powermon.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "devices.h"
#include "dev_types.h"
#include "dev_mcu.h"
#include "dev_leds.h"
#include "cpu_cycle.h"

#include "app_shared_data.h"

static const int pmThreadStackSize = 1000;

uint32_t pmLoopCount = 0;

PmState pmState = PM_STATE_INIT;

static const uint32_t sensorReadInterval = 100;
static uint32_t sensorReadTick = 0;

static const uint32_t thermReadInterval = 300;
static uint32_t thermReadTick = 0;

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return HAL_GetTick() - stateStartTick;
}

static void task_pm (void)
{
    pmLoopCount++;
    const PmState oldState = pmState;
    int pgood = dev_readPgood(&dev.pm);
    int power_5v_ok = (pgood
                    && (dev.pm.monState == MON_STATE_READ)
                    && (getSensorIsValid_5V(&dev.pm)));
    int power_all_ok = (pgood
                    && (dev.pm.monState == MON_STATE_READ)
                    && (pm_sensors_getStatus(&dev.pm)) <= SENSOR_WARNING);
;
    switch (pmState) {
    case PM_STATE_INIT:
        struct_powermon_init(&dev.pm);
        struct_Devices_init(&dev);
        pmState = PM_STATE_STANDBY;
        break;
    case PM_STATE_STANDBY:
        if (stateTicks() > 1000) {
            dev_switchPower(&dev.pm, SWITCH_ON);
            pmState = PM_STATE_RAMP_5V;
        }
        break;
    case PM_STATE_RAMP_5V:
        if (power_5v_ok) {
            pmState = PM_STATE_RAMP;
        }
        if (stateTicks() > 1000) {
            printf("RAMP_5V timeout\n");
            pmState = PM_STATE_PWRFAIL;
        }
        break;
    case PM_STATE_RAMP:
        if (pgood && dev.pm.monState == MON_STATE_READ) {
            if (pm_sensors_getStatus(&dev.pm) <= SENSOR_WARNING) {
                pmState = PM_STATE_RUN;
            }
        }
        if (stateTicks() > 1000) {
            printf("RAMP timeout\n");
            pmState = PM_STATE_PWRFAIL;
        }
        break;
    case PM_STATE_RUN:
        if (!power_all_ok) {
                printf("Power failure in RUN\n");
                pmState = PM_STATE_PWRFAIL;
                break;
        }
        if (dev.pm.monState != MON_STATE_READ) {
                printf("Error in STATE_RUN\n");
                pmState = PM_STATE_ERROR;
                break;
        }
        break;
    case PM_STATE_PWRFAIL:
        dev_switchPower(&dev.pm, SWITCH_OFF);
        dev_readPgood(&dev.pm);
        if (stateTicks() > 2000) {
            pmState = PM_STATE_STANDBY;
        }
        break;
    case PM_STATE_ERROR:
        dev_switchPower(&dev.pm, SWITCH_OFF);
        if (stateTicks() > 2000) {
            pmState = PM_STATE_STANDBY;
        }
        break;
    }

    dev_led_set(&dev.leds, LED_RED, pmState == PM_STATE_PWRFAIL || pmState == PM_STATE_ERROR);
    dev_led_set(&dev.leds, LED_YELLOW, pmState != PM_STATE_RUN);

    if (pmState == PM_STATE_RAMP_5V
            || pmState == PM_STATE_RAMP) {
        runMon(&dev.pm);
    } else
        if (pmState == PM_STATE_RUN) {
            uint32_t ticks = osKernelSysTick() - sensorReadTick;
            if (ticks > sensorReadInterval) {
                sensorReadTick = osKernelSysTick();
                runMon(&dev.pm);
            }

        }
        else  {
            struct_powermon_init(&dev.pm);
        }
    if ((pmState == PM_STATE_RUN)
            && getSensorIsValid_5V(&dev.pm)) {
        uint32_t ticks = osKernelSysTick() - thermReadTick;
        if (ticks > thermReadInterval) {
            thermReadTick = osKernelSysTick();
            dev_read_thermometers(&dev);
        }
    } else {
        struct_thset_init(&dev.thset);
    }

    if (oldState != pmState) {
        stateStartTick = HAL_GetTick();
    }
}

static void unused1(void)
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

static void prvPowermonTask( void const *arg)
{
    (void) arg;
    while (1)
    {
        task_pm();
        osDelay(2);
    }
}

osThreadDef(powermonThread, prvPowermonTask, osPriorityHigh,      1, pmThreadStackSize);

void create_task_powermon(void)
{
    osThreadId threadId = osThreadCreate(osThread (powermonThread), NULL);
    if (threadId == NULL) {
        printf("Failed to create Powermon thread\n");
    }
}
