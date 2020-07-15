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

typedef struct pm_switches {
    bool switch_5v;
    bool switch_3v3;
    bool switch_1v5;
    bool switch_1v0;
    bool switch_tdc_a;
    bool switch_tdc_b;
    bool switch_tdc_c;
} pm_switches;

typedef struct pm_pgoods {
   bool fpga_core_pgood;
   bool ltm_pgood;
} pm_pgoods;

#ifdef __cplusplus
}
#endif

#endif // BSP_POWERMON_TYPES_H
