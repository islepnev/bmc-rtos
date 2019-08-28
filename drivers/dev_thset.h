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
#ifndef DEV_THSET_H
#define DEV_THSET_H

#include <stdint.h>

#include "dev_common_types.h"

typedef enum {
    THSET_STATE_0,
    THSET_STATE_1,
    THSET_STATE_2,
} thset_state_t;

enum {DEV_THERM_COUNT = 4};

typedef struct Dev_adt7301 {
    int valid;
    int16_t rawTemp;
} Dev_adt7301;

typedef struct Dev_thset {
    thset_state_t state;
    Dev_adt7301 th[DEV_THERM_COUNT];
} Dev_thset;

void dev_thset_init(Dev_thset *d);
void dev_thset_run(Dev_thset *d);
//void dev_thset_read(Dev_thset *d);
//void dev_print_thermometers(const Devices *dev);

//void dev_thset_read(Dev_thset *d);
//void dev_thset_print(const Dev_thset d);
//SensorStatus dev_adt7301_status(const Dev_adt7301 *d);
SensorStatus dev_thset_thermStatus(const Dev_thset *d);
int16_t adt7301_convert_temp_adt7301_scale32(int16_t raw);
thset_state_t thermal_shutdown_check(Dev_thset *d);
void clear_thermal_shutdown(Dev_thset *d);

#endif // DEV_THSET_H
