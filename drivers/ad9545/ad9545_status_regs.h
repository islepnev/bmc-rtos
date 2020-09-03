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

#ifndef AD9545_STATUS_REGS_H
#define AD9545_STATUS_REGS_H

#include <stdint.h>

typedef union
{
  struct
  {
      uint8_t save_in_progress:1;
      uint8_t load_in_progress:1;
      uint8_t fault:1;
      uint8_t crc_error:1;
      uint8_t reserved:4;
  } b;
  uint8_t raw;
} AD9545_Eeprom_Status_REG_Type;

typedef union
{
  struct
  {
      uint8_t locked:1;
      uint8_t stable:1;
      uint8_t cal_busy:1;
      uint8_t reserved:1;
      uint8_t pll0_locked:1;
      uint8_t pll1_locked:1;
      uint8_t reserved2:2;
  } b;
  uint8_t raw;
} AD9545_Sysclk_Status_REG_Type;

typedef union
{
  struct
  {
      uint8_t temp_alarm:1;
      uint8_t aux_dpll_lock:1;
      uint8_t aux_dpll_ref_fault:1;
      uint8_t reserved:1;
      uint8_t aux_nco0_delta_slewing:1;
      uint8_t aux_nco0_delta_overflow:1;
      uint8_t aux_nco1_delta_slewing:1;
      uint8_t aux_nco1_delta_overflow:1;
  } b;
  uint8_t raw;
} AD9545_Misc_Status_REG_Type;

typedef union
{
  struct
  {
      uint8_t slow:1;
      uint8_t fast:1;
      uint8_t jitter:1;
      uint8_t fault:1;
      uint8_t valid:1;
      uint8_t los:1;
      uint8_t reserved:2;
  } b;
  uint8_t raw;
} AD9545_Ref_Status_REG_Type;

typedef union
{
  struct
  {
      uint8_t profile_0:1;
      uint8_t profile_1:1;
      uint8_t profile_2:1;
      uint8_t profile_3:1;
      uint8_t profile_4:1;
      uint8_t profile_5:1;
      uint8_t reserved:2;
  } b;
  uint8_t raw;
} AD9545_DPLL_Active_Profile_REG_Type;

typedef union
{
  struct
  {
      uint8_t all_lock:1;
      uint8_t dpll_phase_lock:1;
      uint8_t dpll_freq_lock:1;
      uint8_t apll_lock:1;
      uint8_t apll_cal_busy:1;
      uint8_t apll_cal_done:1;
      uint8_t reserved:2;
  } b;
  uint8_t raw;
} AD9545_DPLL_Lock_Status_REG_Type;

typedef union
{
  struct
  {
      uint8_t freerun:1;
      uint8_t holdover:1;
      uint8_t ref_switch:1;
      uint8_t active:1;
      uint8_t active_profile:3;
      uint8_t reserved:1;
  } b;
  uint8_t raw;
} AD9545_DPLL_Operation_REG_Type;

typedef union
{
  struct
  {
      uint8_t hist_available:1;
      uint8_t freq_clamp:1;
      uint8_t phase_slew_limit:1;
      uint8_t reserved1:1;
      uint8_t facq_active:1;
      uint8_t facq_done:1;
      uint8_t reserved:2;
  } b;
  uint8_t raw;
} AD9545_DPLL_State_REG_Type;

typedef struct {
    AD9545_DPLL_Active_Profile_REG_Type act_profile;
    AD9545_DPLL_Lock_Status_REG_Type lock_status;
    AD9545_DPLL_Operation_REG_Type operation;
    AD9545_DPLL_State_REG_Type state;
    uint64_t ftw_history;
    uint16_t pld_tub;
    uint16_t fld_tub;
    uint8_t phase_slew;
    uint8_t phase_control_error;
} AD9545_DPLL_Status;

enum { REF_INPUT_COUNT = 4};
enum { DPLL_COUNT = 2};

typedef struct {
    AD9545_Eeprom_Status_REG_Type eeprom;
    AD9545_Sysclk_Status_REG_Type sysclk;
    AD9545_Misc_Status_REG_Type misc;
    int16_t internal_temp;
    AD9545_Ref_Status_REG_Type ref[REF_INPUT_COUNT];
    AD9545_DPLL_Status dpll[DPLL_COUNT];
    uint32_t pll_unlock_cntr;
} AD9545_Status;

#endif // AD9545_STATUS_REGS_H
