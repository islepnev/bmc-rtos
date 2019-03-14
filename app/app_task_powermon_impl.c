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
#include "i2c.h"

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
#include "logbuffer.h"
#include "app_shared_data.h"

const int TEST_RESTART = 0; // debug only
const uint32_t SENSORS_SETTLE_TICKS = 200;
const uint32_t THERM_SETTLE_TICKS = 1000;
const uint32_t RAMP_TIMEOUT_TICKS = 3000;
const uint32_t RAMP_5V_TIMEOUT_TICKS = 3000;
const uint32_t POWERFAIL_DELAY_TICKS = 3000;
const uint32_t ERROR_DELAY_TICKS = 3000;

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

SensorStatus oldSensorStatus[POWERMON_SENSORS] = {0};
void clearOldSensorStatus(void)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        oldSensorStatus[i] = SENSOR_NORMAL;
}

static const char *sensorStatusStr(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return "  UNKNOWN";
    case SENSOR_NORMAL:   return "  NORMAL";
    case SENSOR_WARNING:  return " WARNING";
    case SENSOR_CRITICAL: return "CRITICAL";
    default: return "FAIL";
    }
}

static void log_sensor_status(const Dev_powermon *pm)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &pm->sensors[i];
        SensorStatus status = pm_sensor_status(sensor);
        if (status != oldSensorStatus[i]) {
            enum { size = 50 };
            static char str[size];
            const float volt = sensor->busVoltage;
            const int volt_int = volt;
            int volt_frac = 1000 * (volt - volt_int);
            if (volt_frac < 0) volt_frac = -volt_frac;
            float curr = sensor->current;
            int neg = (curr < 0);
            if (neg) curr = -curr;
            const int curr_int = curr;
            int curr_frac = 1000 * (curr - curr_int);
            snprintf(str, size, "%s %s, %d.%03d V, %s%d.%03d A",
                     sensor->label,
                     sensorStatusStr(status),
                     volt_int,
                     volt_frac,
                     neg?"-":"",
                     curr_int,
                     curr_frac
                     );
            switch(status) {
            case SENSOR_UNKNOWN:
                log_put(LOG_WARNING, str);
                break;
            case SENSOR_CRITICAL:
                log_put(LOG_ERR, str);
                break;
            case SENSOR_WARNING:
                log_put(LOG_WARNING, str);
                break;
            case SENSOR_NORMAL:
                log_put(LOG_INFO, str);
                break;
            }
            oldSensorStatus[i] = status;
        }
    }
}

SensorStatus getMonStatus(const Dev_powermon *pm)
{
    SensorStatus monStatus = SENSOR_CRITICAL;
    if ((pm->monState == MON_STATE_READ)
            && (getMonStateTicks(pm) > SENSORS_SETTLE_TICKS)) {
        monStatus = pm_sensors_getStatus(pm);
    }
    return monStatus;
}

static int pm_initialized = 0;

void powermon_task (void)
{
    Dev_powermon *pm = &dev.pm;
    if (!pm_initialized) {
        clearOldSensorStatus();
        pm_initialized = 1;
    }
    pmLoopCount++;
    int vmePresent = 1; // pm_read_liveInsert(&dev.pm);
    const PmState oldState = pmState;
    int pgood = dev_readPgood(pm);
    int power_5v_ok = (pgood
                    && (pm->monState == MON_STATE_READ)
                       && (getSensorIsValid_5V(pm))
                       );
    int power_all_ok = (pgood
                    && (pm->monState == MON_STATE_READ)
                    && (pm_sensors_getStatus(pm)) <= SENSOR_WARNING);
    const SensorStatus monStatus = getMonStatus(pm);
    switch (pmState) {
    case PM_STATE_INIT:
        struct_powermon_init(pm);
        struct_Devices_init(&dev);
        pmState = PM_STATE_RAMP_5V;
        break;
    case PM_STATE_STANDBY:
        struct_powermon_init(pm);
        struct_Devices_init(&dev);
        if (vmePresent && (stateTicks() > 2000)) {
            pmState = PM_STATE_RAMP_5V;
        }
        break;
    case PM_STATE_RAMP_5V:
        if (!vmePresent) {
            pmState = PM_STATE_STANDBY;
            break;
        }
        if (power_5v_ok) {
            log_put(LOG_NOTICE, "5V Ok");
            pmState = PM_STATE_RAMP;
            break;
        }
        if (stateTicks() > RAMP_5V_TIMEOUT_TICKS) {
            if (!pm->fpga_core_pgood && !pm->ltm_pgood) {
                log_put(LOG_WARNING, "No power");
                pmState = PM_STATE_STANDBY;
                break;
            }
            log_put(LOG_ERR, "RAMP_5V timeout");
            pmState = PM_STATE_PWRFAIL;
        }
        break;
    case PM_STATE_RAMP:
        if (!vmePresent) {
            pmState = PM_STATE_STANDBY;
            break;
        }
        if (pgood
                && (monStatus <= SENSOR_WARNING)) {
            log_put(LOG_NOTICE, "power supplies ready");
            pmState = PM_STATE_RUN;
        }
        if (stateTicks() > RAMP_TIMEOUT_TICKS) {
            log_put(LOG_ERR, "RAMP timeout");
            pmState = PM_STATE_PWRFAIL;
        }
        break;
    case PM_STATE_RUN:
        if (!vmePresent) {
            pmState = PM_STATE_STANDBY;
            break;
        }
        if (!power_all_ok) {
            log_put(LOG_ERR, "Power failure in RUN");
            pmState = PM_STATE_PWRFAIL;
            break;
        }
        if (pm->monState != MON_STATE_READ) {
            log_put(LOG_ERR, "Error in STATE_RUN");
            pmState = PM_STATE_ERROR;
            break;
        }
        if (TEST_RESTART && (stateTicks() > 5000)) {
            pmState = PM_STATE_STANDBY;
            break;
        }
        break;
    case PM_STATE_PWRFAIL:
//        dev_switchPower(&dev.pm, SWITCH_OFF);
//        dev_readPgood(&dev.pm);
        if (stateTicks() > POWERFAIL_DELAY_TICKS) {
            pmState = PM_STATE_STANDBY;
        }
        break;
    case PM_STATE_ERROR:
//        dev_switchPower(&dev.pm, SWITCH_OFF);
        if (stateTicks() > ERROR_DELAY_TICKS) {
            pmState = PM_STATE_STANDBY;
        }
        break;
    }

    if ((pmState == PM_STATE_RAMP_5V)
            || (pmState == PM_STATE_RAMP)
            || (pmState == PM_STATE_RUN)) {
        dev_switchPower(pm, SWITCH_ON);
    } else {
        dev_switchPower(pm, SWITCH_OFF);
    }

    if ((pmState == PM_STATE_RAMP_5V)
            || (pmState == PM_STATE_RAMP)) {
        runMon(pm);
    } else {
        if (pmState == PM_STATE_RUN) {
            uint32_t ticks = osKernelSysTick() - sensorReadTick;
            if (ticks > sensorReadInterval) {
                sensorReadTick = osKernelSysTick();
                runMon(pm);
            }

        }
        else  {
            monClearMeasurements(pm);
        }
    }
    if ((pmState == PM_STATE_RAMP)
            || (pmState == PM_STATE_RUN)
            ) {
        log_sensor_status(pm);
    } else {
        clearOldSensorStatus();
    }
    if ((pmState == PM_STATE_RUN)
            && (getMonStateTicks(pm) > THERM_SETTLE_TICKS)
            && getSensorIsValid_3V3(pm)) {
        uint32_t ticks = osKernelSysTick() - thermReadTick;
        if (ticks > thermReadInterval) {
            thermReadTick = osKernelSysTick();
            dev_thset_read(&dev.thset);
        }
    } else {
        struct_thset_init(&dev.thset);
    }

    if (oldState != pmState) {
        stateStartTick = osKernelSysTick();
    }
}

//static void unused1(void)
//{
//    enable_cpu_cycle_counter();
//    const uint32_t startTick = cpu_getCycles();
//    uint32_t time1 = cpu_getCycles();
//    uint32_t time2 = cpu_getCycles();
//    uint32_t time3 = cpu_getCycles();
//    uint32_t time4 = cpu_getCycles();
//    time1 -= startTick;
//    time2 -= startTick;
//    time3 -= startTick;
//    time4 -= startTick;
//    printf(ANSI_CLEAR "%-8ld %-8ld %-8ld %-8ld \n",
//           (uint32_t)((float)time1 * 1e3f / HAL_RCC_GetHCLKFreq()),
//           (uint32_t)((float)time2 * 1e3f / HAL_RCC_GetHCLKFreq()),
//           (uint32_t)((float)time3 * 1e3f / HAL_RCC_GetHCLKFreq()),
//           (uint32_t)((float)time4 * 1e3f / HAL_RCC_GetHCLKFreq())
//           );
//}
