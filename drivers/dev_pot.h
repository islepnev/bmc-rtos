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
#ifndef DEV_POT_H
#define DEV_POT_H

#include <stdint.h>
#include "dev_types.h"
#include "dev_pm_sensors_config.h"

#ifdef TDC64
enum {DEV_POT_COUNT = 4};
#else
enum {DEV_POT_COUNT = 3};
#endif
typedef enum {
    POT_TDC_A,
    POT_TDC_B,
    POT_TDC_C
#ifdef TDC64
    , POT_TDC_D
#endif
} PotIndex;

typedef struct Dev_ad5141 {
    PotIndex index;
    SensorIndex sensorIndex;
    uint8_t busAddress;
    DeviceStatus deviceStatus;
    uint8_t value;
} Dev_ad5141;

typedef struct Dev_pots {
    Dev_ad5141 pot[DEV_POT_COUNT];
} Dev_pots;

extern int pot_screen_selected;

void struct_pots_init(Dev_pots *d);
int pot_detect(Dev_pots *d);
void pot_read_rdac_all(Dev_pots *d);
const char *potLabel(PotIndex index);
void dev_ad5141_reset(Dev_ad5141 *d);
void dev_ad5141_inc(Dev_ad5141 *d);
void dev_ad5141_dec(Dev_ad5141 *d);
void dev_ad5141_write(Dev_ad5141 *d);

#endif // DEV_POT_H
