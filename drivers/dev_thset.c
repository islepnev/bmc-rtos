/*
**    Copyright 2019 Ilja Slepnev
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

#include "dev_thset.h"

#include "dev_thset_types.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "bsp_thset.h"
#include "logbuffer.h"

static const uint32_t thermReadInterval = 300;
static uint32_t thermReadTick = 0;

// Temperature limits
static const int16_t tempMinValid = -40;
static const int16_t tempMaxValid = 150;
static const int16_t tempMinCrit = -30;
static const int16_t tempMaxCrit = 80;
static const int16_t tempMinWarn = 0;
static const int16_t tempMaxWarn = 60;

static void struct_thset_init(Dev_thset *d)
{
    d->state = THSET_STATE_0;
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].valid = 0;
        d->th[i].rawTemp = 0;
    }
}

void dev_thset_read(Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t rawTemp;
        HAL_StatusTypeDef ret = adt7301_read_temp(i, &rawTemp);
        int16_t temp = rawTemp / 32;
        d->th[i].valid = (ret == HAL_OK) && (temp > tempMinValid) && (temp < tempMaxValid);
        d->th[i].rawTemp = rawTemp;
    }
}

SensorStatus dev_adt7301_status(const Dev_adt7301 *d)
{
    if (!d->valid)
        return SENSOR_UNKNOWN;
    int16_t temp = d->rawTemp;
    temp /= 32;
    if (temp < tempMinCrit || temp > tempMaxCrit) {
        return SENSOR_CRITICAL;
    }
    if (temp < tempMinWarn || temp > tempMaxWarn) {
        return SENSOR_WARNING;
    }
    return SENSOR_NORMAL;
}

SensorStatus dev_thset_thermStatus(const Dev_thset *d)
{
    SensorStatus maxStatus = SENSOR_UNKNOWN;
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        const SensorStatus status = dev_adt7301_status(&d->th[i]);
        if (status > maxStatus)
            maxStatus = status;
    }
    return maxStatus;
}

static uint32_t thermal_shutdown_start_tick = 0;
static const int thermal_shutdown_min_period_ticks = 5000;

thset_state_t thermal_shutdown_check(Dev_thset *d)
{
    int valid_count = 0;
    int critical_count = 0;
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        if (d->th[i].valid) {
            valid_count++;
            int16_t temp = d->th[i].rawTemp / 32;
            if (temp > tempMaxCrit)
                critical_count++;
        }
    }
    // minimum 2 sensors
    if (valid_count < 2)
        return d->state;
    const int shutdown_condition = (critical_count >= 2);
    const int recover_condition = (valid_count == DEV_THERM_COUNT) && (critical_count == 0);
    switch (d->state) {
    case THSET_STATE_0:
        if (shutdown_condition) {
            log_printf(LOG_WARNING, "Temperature warning (%d of %d sensors critical)", critical_count, valid_count);
            thermal_shutdown_start_tick = osKernelSysTick();
            d->state = THSET_STATE_1;
        }
        break;
    case THSET_STATE_1:
        if (recover_condition) {
            d->state = THSET_STATE_0;
            break;
        }
        uint32_t period = osKernelSysTick() - thermal_shutdown_start_tick;
        if (period >= thermal_shutdown_min_period_ticks) {
            log_put(LOG_CRIT, "Temperature critical, shutdown");
            for (int i=0; i < DEV_THERM_COUNT; i++) {
                int16_t temp = d->th[i].rawTemp;
                log_printf(LOG_CRIT, "Temperature sensor [%d]: %d", i, temp / 32);
            }
            d->state = THSET_STATE_2;
        }
        break;
    case THSET_STATE_2:
        if (recover_condition) {
            log_put(LOG_NOTICE, "Recovering from thermal shutdown");
            d->state = THSET_STATE_0;
            break;
        }
        break;
    default:
        break;
    }
    return d->state;
}

void clear_thermal_shutdown(Dev_thset *d)
{
    d->state = THSET_STATE_0;
}

void dev_thset_init(Dev_thset *d)
{
    struct_thset_init(d);
}

void dev_thset_run(Dev_thset *d)
{
   uint32_t ticks = osKernelSysTick() - thermReadTick;
   if (ticks > thermReadInterval) {
       thermReadTick = osKernelSysTick();
       dev_thset_read(d);
       int allzero = 1;
       for (int i=0; i < DEV_THERM_COUNT; i++)
           if (d->th[i].valid && d->th[i].rawTemp != 0)
               allzero = 0;
       if (allzero)
           for (int i=0; i < DEV_THERM_COUNT; i++)
               d->th[i].valid = 0;
   }
   thermal_shutdown_check(d);
}
