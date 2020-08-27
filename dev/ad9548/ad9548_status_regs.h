/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef AD9548_STATUS_REGS_H
#define AD9548_STATUS_REGS_H

#include "ad9548_regs.h"

typedef union {
    struct {
        uint8_t locked:1;
        uint8_t cal_busy:1;
        uint8_t reserved1:2;
        uint8_t stable:1;
        uint8_t reserved2:3;
    } b;
    uint8_t raw;
} AD9548_Sysclk_Status_REG_Type;

typedef union
{
    struct
    {
        uint8_t slow:1;
        uint8_t fast:1;
        uint8_t fault:1;
        uint8_t valid:1;
        uint8_t prof_index:3;
        uint8_t prof_selected:1;
    } b;
    uint8_t raw;
} AD9548_Ref_Status_REG_Type;

typedef union {
    struct {
        uint8_t freerun:1;
        uint8_t active:1;
        uint8_t holdover:1;
        uint8_t ref_switch:1;
        uint8_t dpll_phase_lock:1;
        uint8_t dpll_freq_lock:1;
        uint8_t phase_buildout:1;
        uint8_t phase_slew_limit:1;
    } b;
    uint8_t raw;
} AD9548_DPLL_Status_1_REG_Type;

typedef union {
    struct {
        uint8_t active_ref:3;
        uint8_t active_ref_prio:3;
        uint8_t hist_available:1;
        uint8_t freq_clamp:1;
    } b;
    uint8_t raw;
} AD9548_DPLL_Status_2_REG_Type;

typedef struct AD9548_Status
{
    // bool isOk() const;
    AD9548_Sysclk_Status_REG_Type sysclk;
    AD9548_DPLL_Status_1_REG_Type DpllStat;
    AD9548_DPLL_Status_2_REG_Type DpllStat2;
    uint8_t refPowerDown;
    //uint8_t refActive;
    AD9548_Ref_Status_REG_Type refStatus[8];
    uint64_t holdover_ftw;
} AD9548_Status;

#endif // AD9548_STATUS_REGS_H
