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

// Temperature limits
static const int tempMinCrit = -40;
static const int tempMaxCrit = 80.0;
static const int tempMinWarn = 0.1;
static const int tempMaxWarn = 60.0;

void struct_thset_init(Dev_thset *d)
{
    for (int i=0; i<DEV_THERM_COUNT; i++) {
        d->th[i].valid = 0;
        d->th[i].rawTemp = 0;
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
        int16_t temp = adt7301_convert_temp_adt7301_scale32(d->th[i].rawTemp);
        temp /= 32;
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
