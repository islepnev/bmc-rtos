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
#include "adt7301_spi_hal.h"
#include "max31725_i2c_hal.h"
#include "tmp421_i2c_hal.h"

// Temperature limits
static const double tempMinCrit = -40;
static const double tempMaxCrit = 80;
static const double tempMinWarn = 0.1;
static const double tempMaxWarn = 60;

void struct_thset_init(Dev_thset *d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].valid = 0;
        d->th[i].rawTemp = 0;
        d->th[i].temp = 0;
    }
}

/**
  * @brief  Converts raw temperature value
  * @param  raw: 14-bit raw value from ADT7301 temperature sensor
  * @retval temperature in degrees Celsius multiplied by 32
  */
int16_t adt7301_convert_temp_adt7301_scale32(int16_t raw)
{
    return (int16_t)(raw << 2) >> 2;
}

SensorStatus dev_thset_thermStatus(const Dev_thset *d)
{
    SensorStatus maxStatus = SENSOR_NORMAL;
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        if (!d->th[i].valid)
            continue;
        double temp = d->th[i].temp;
        if (temp < tempMinCrit || temp > tempMaxCrit) {
            if (SENSOR_CRITICAL > maxStatus)
                maxStatus = SENSOR_CRITICAL;
        }
        if (temp < tempMinWarn || temp > tempMaxWarn) {
            if (SENSOR_WARNING > maxStatus)
                maxStatus = SENSOR_WARNING;
        }
    }
    return maxStatus;
}

void dev_thset_read(Dev_thset *d)
{
#ifdef TTVXS_1_0
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t rawTemp;
        HAL_StatusTypeDef ret = adt7301_read_temp(i, &rawTemp);
        d->th[i].valid = (ret == HAL_OK);
        d->th[i].rawTemp = rawTemp;
    }
#endif
    max31725_read(&d->th[0]);
    tmp421_read(&d->th[1]);
}
