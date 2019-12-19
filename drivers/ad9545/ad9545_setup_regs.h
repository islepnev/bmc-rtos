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

#ifndef AD9545_SETUP_REGS_H
#define AD9545_SETUP_REGS_H

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
    uint16_t Temperature_High_Threshold;
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

enum {
    PROFILE_PRIORITY_DISABLED = 0,
    PROFILE_PRIORITY_HIGH = 0x1 | (0x00 << 1),
    PROFILE_PRIORITY_NORMAL  = 0x1 | (0x0F << 1),
    PROFILE_PRIORITY_LOW  = 0x1 | (0x1F << 1)
};

//enum {
//    PROFILE_FEEDBACK_MODE_PHASE_BUILDOUT = 0,
//    PROFILE_FEEDBACK_MODE_INT_ZD = 2,
//    PROFILE_FEEDBACK_MODE_EXT_ZD = 3,
//};

enum {
    PROFILE_INT_ZD_FEEDBACK_OUT0A  = 0,
    PROFILE_INT_ZD_FEEDBACK_OUT0AN = 1,
    PROFILE_INT_ZD_FEEDBACK_OUT0B  = 2,
    PROFILE_INT_ZD_FEEDBACK_OUT0BN = 3,
    PROFILE_INT_ZD_FEEDBACK_OUT0C  = 4,
    PROFILE_INT_ZD_FEEDBACK_OUT0CN = 5
};

enum {
    PROFILE_INT_ZD_FEEDBACK_OUT1A  = 0,
    PROFILE_INT_ZD_FEEDBACK_OUT1AN = 1,
    PROFILE_INT_ZD_FEEDBACK_OUT1B  = 2,
    PROFILE_INT_ZD_FEEDBACK_OUT1BN = 3
};

enum {
    PROFILE_EXT_ZD_FEEDBACK_REFA   = 0,
    PROFILE_EXT_ZD_FEEDBACK_REFAA  = 1,
    PROFILE_EXT_ZD_FEEDBACK_REFB   = 2,
    PROFILE_EXT_ZD_FEEDBACK_REFBB  = 3
};

enum {
    DPLL_MODE_PROFILE_SELECT_AUTOMATIC = 0,
    DPLL_MODE_PROFILE_SELECT_FALLBACK_PRIORITY = 1,
    DPLL_MODE_PROFILE_SELECT_FALLBACK_HOLDOVER = 2,
    DPLL_MODE_PROFILE_SELECT_MANUAL = 3
};

typedef struct ad9545_setup_t {
    Pll_OutputDrivers_Setup_TypeDef out_drivers;
    Pll_OutputDividers_Setup_TypeDef out_dividers;
    Pll_DPLL_Setup_TypeDef dpll0;
    Pll_DPLL_Setup_TypeDef dpll1;
    Pll_DPLLMode_Setup_TypeDef dpll_mode;
    PllRefSetup_TypeDef ref;
    PllSysclkSetup_TypeDef sysclk;
} ad9545_setup_t;

#endif // AD9545_SETUP_REGS_H
