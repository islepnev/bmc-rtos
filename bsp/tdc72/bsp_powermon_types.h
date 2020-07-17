/*
**    Copyright 2019-2020 Ilja Slepnev
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
#ifndef BSP_POWERMON_TYPES_H
#define BSP_POWERMON_TYPES_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PowerSwitchIndex {
    PSW_5V,
    PSW_3V3,
    PSW_1V5,
    PSW_1V0,
    PSW_TDC_A,
    PSW_TDC_B,
    PSW_TDC_C
} PowerSwitchIndex;

#define POWER_SWITCH_COUNT 7
const char *psw_label(PowerSwitchIndex index);
typedef bool pm_switches[POWER_SWITCH_COUNT];

typedef enum PowerGoodIndex {
    PGOOD_1V5,
    PGOOD_1V0,
} PowerGoodIndex;

#define POWER_GOOD_COUNT 2
const char *pgood_label(PowerGoodIndex index);
typedef bool pm_pgoods[POWER_GOOD_COUNT];

bool pm_switches_isEqual(const pm_switches l, const pm_switches r);

#ifdef __cplusplus
}
#endif

#endif // BSP_POWERMON_TYPES_H
