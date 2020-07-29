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

#include <string.h>

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

bool dev_thset_add(Dev_thset *d, const char *name)
{
    if (d->count >= DEV_THSET_MAX_COUNT)
        return false;
    int i = d->count;
    GenericSensor *sensor = &d->sensors[i];
    sensor->hdr.raw = 0;
    sensor->hdr.b.type = IPMI_SENSOR_TEMPERATURE;
    if (name)
        generic_sensor_set_name(sensor, name);
    d->count++;
    return true;
}

//void dev_thset_read(Dev_thset *d)
//{
//    for(int i=0; i<DEV_THERM_COUNT; i++) {
//        int16_t rawTemp;
//        HAL_StatusTypeDef ret = adt7301_read_temp(i, &rawTemp);
//        int16_t temp = rawTemp / 32;
//        d->th[i].valid = (ret == HAL_OK) && (temp > tempMinValid) && (temp < tempMaxValid);
//        d->th[i].rawTemp = rawTemp;
//    }
//}

SensorStatus dev_thset_sensor_status(const GenericSensor *d)
{
    if (d->hdr.b.state != DEVICE_NORMAL)
        return SENSOR_UNKNOWN;
    double temp = d->value;
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
    for (int i=0; i < d->count; i++) {
        const SensorStatus status = dev_thset_sensor_status(&d->sensors[i]);
        if (status > maxStatus)
            maxStatus = status;
    }
    return maxStatus;
}

static uint32_t thermal_shutdown_start_tick = 0;
static const int thermal_shutdown_min_period_ticks = 5000;

void dev_thset_run(Dev_thset *d)
{
    int valid_count = 0;
    int critical_count = 0;
    for (int i=0; i < d->count; i++) {
        if (d->sensors[i].hdr.b.state == DEVICE_NORMAL) {
            valid_count++;
            double temp = d->sensors[i].value;
            if (temp > tempMaxCrit)
                critical_count++;
        }
    }
    // minimum 2 sensors
    if (valid_count < 2)
        return;
    const int shutdown_condition = (critical_count >= 2);
    const int recover_condition = (valid_count == d->count) && (critical_count == 0);
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
        {
            uint32_t period = osKernelSysTick() - thermal_shutdown_start_tick;
            if (period >= thermal_shutdown_min_period_ticks) {
                log_put(LOG_CRIT, "Temperature critical, shutdown");
                for (int i=0; i < d->count; i++) {
                    double temp = d->sensors[i].value;
                    log_printf(LOG_CRIT, "Temperature sensor [%d]: %f", i, temp);
                }
                d->state = THSET_STATE_2;
            }
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
}

void clear_thermal_shutdown(Dev_thset *d)
{
    d->state = THSET_STATE_0;
}
