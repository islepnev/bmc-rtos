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
#ifndef AD9545_SETUP_H
#define AD9545_SETUP_H

#include <stdint.h>

typedef enum {
    REFA = 0,
    REFAA = 1,
    REFB = 2,
    REFBB = 3,
} PllRef_TypeDef;

typedef enum {
    DPLL0 = 0,
    DPLL1 = 1,
} PllChannel_TypeDef;

typedef struct {
    uint8_t Sysclk_FB_DIV_Ratio;
    uint8_t Sysclk_Input;
    uint64_t sysclk_Ref_Frequency_milliHz;
    uint32_t Sysclk_Stability_Timer;
    uint16_t Temperature_Low_Threshold;
    uint16_t Temperature_Hihg_Threshold;
    uint16_t AuxDPLL_Bandwidth;
    int64_t CompensationValue;
    uint8_t TDC_Compensation_Source;
    uint8_t DPLL_Compensation_Source;
} PllSysclkSetup_TypeDef;

typedef struct {
    uint8_t REFA_Receiver_Settings;
    uint32_t REFA_R_Divider;
    uint64_t REFA_Input_Period;
    uint32_t REFA_Offset_Limit;
    uint32_t REFA_Validation_Timer;
    uint32_t REFA_Jitter_Tolerance;
    uint8_t REFB_Receiver_Settings;
    uint32_t REFB_R_Divider;
    uint64_t REFB_Input_Period;
    uint32_t REFB_Offset_Limit;
    uint32_t REFB_Validation_Timer;
    uint16_t REFB_Jitter_Tolerance;
} PllRefSetup_TypeDef;

typedef union
{
  struct
  {
      uint8_t enable_hitless:1;
      uint8_t enable_ext_zd:1;
      uint8_t tag_mode:3;
      uint8_t reserved:2;
      uint8_t loopfilter_base:1;
  } b;
  uint8_t raw;
} DPLL_Feedback_Mode_REG_Type;

typedef struct {
    uint8_t Priority_and_Enable;
    uint8_t Profile_Ref_Source;
    uint8_t ZD_Feedback_Path;
    DPLL_Feedback_Mode_REG_Type Feedback_Mode;
    uint32_t Loop_BW;
    uint32_t Hitless_FB_Divider;
    uint32_t Buildout_FB_Divider;
    uint32_t Buildout_FB_Fraction;
    uint32_t Buildout_FB_Modulus;
    uint32_t FastLock;
} Pll_DPLL_Profile_TypeDef;

enum {DPLL_PROFILE_COUNT = 6};
typedef struct {
    uint64_t Freerun_Tuning_Word;
    uint32_t FTW_Offset_Clamp;
    uint8_t APLL_M_Divider;
    Pll_DPLL_Profile_TypeDef profile[DPLL_PROFILE_COUNT];
} Pll_DPLL_Setup_TypeDef;

typedef union
{
  struct
  {
    uint32_t enable_hcsl:1;
    uint32_t driver_current:2;
    uint32_t driver_mode:2;
    uint32_t bypass_retiming:1;
    uint32_t reserved:2;
  } b;
  uint8_t raw;
} Driver_Config_REG_Type;

typedef struct {
    Driver_Config_REG_Type Driver_Config;
} Pll_OutputDrivers_Setup_TypeDef;

typedef union
{
  struct
  {
    uint32_t force_freerun:1;
    uint32_t force_holdover:1;
    uint32_t tr_prof_select_mode:2;
    uint32_t assign_tr_prof:3;
    uint32_t enable_step_detect_ref_fault:1;
  } b;
  uint8_t raw;
} DPLL_Mode_REG_Type;

typedef struct {
    DPLL_Mode_REG_Type dpll0_mode;
    DPLL_Mode_REG_Type dpll1_mode;
} Pll_DPLLMode_Setup_TypeDef;

typedef enum {
    AutosyncMode_Manual = 0,
    AutosyncMode_Immediate = 1,
    AutosyncMode_DPLL_Phase_Lock = 2,
    AutosyncMode_DPLL_Freq_Lock = 3,
} AutosyncMode_TypeDef;

typedef struct {
    uint8_t enable_ref_sync_0;
    uint8_t enable_ref_sync_1;
    AutosyncMode_TypeDef autosync_mode_0;
    AutosyncMode_TypeDef autosync_mode_1;
    uint8_t Secondary_Clock_Path_0;
    uint8_t Secondary_Clock_Path_1;
    uint8_t Automute_Control_0;
    uint8_t Automute_Control_1;
    uint8_t Distribution_Divider_0_A;
    uint8_t Distribution_Divider_0_B;
    uint8_t Distribution_Divider_0_C;
    uint8_t Distribution_Divider_1_A;
    uint8_t Distribution_Divider_1_B;

} Pll_OutputDividers_Setup_TypeDef;

typedef enum {
    PROFILE_REF_SOURCE_A = 0,
    PROFILE_REF_SOURCE_AA = 1,
    PROFILE_REF_SOURCE_B = 2,
    PROFILE_REF_SOURCE_BB = 3,
    PROFILE_REF_SOURCE_DPLL0 = 4,
    PROFILE_REF_SOURCE_DPLL1 = 5,
    PROFILE_REF_SOURCE_NCO0 = 8,
    PROFILE_REF_SOURCE_NCO1 = 9,
    PROFILE_REF_SOURCE_INVALID = 0xF,
} ProfileRefSource_TypeDef;

void init_PllSysclkSetup(PllSysclkSetup_TypeDef *d);
void init_PllRefSetup(PllRefSetup_TypeDef *d);
//void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d);
//void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d);
uint64_t get_dpll_default_ftw(PllChannel_TypeDef channel);
ProfileRefSource_TypeDef get_dpll_default_ref_source(PllChannel_TypeDef channel);
void init_DPLL_Setup(Pll_DPLL_Setup_TypeDef *d, PllChannel_TypeDef channel);
void init_Pll_OutputDrivers_Setup(Pll_OutputDrivers_Setup_TypeDef *d);
void init_Pll_DPLLMode_Setup(Pll_DPLLMode_Setup_TypeDef *d);
void init_Pll_OutputDividers_Setup(Pll_OutputDividers_Setup_TypeDef *d);

#endif // AD9545_SETUP_H
