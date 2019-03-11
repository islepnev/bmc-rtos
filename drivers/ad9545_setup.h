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

typedef struct {
    uint8_t Sysclk_FB_DIV_Ratio;
    uint8_t Sysclk_Input;
    uint64_t sysclk_Ref_Frequency_milliHz;
    uint32_t Sysclk_Stability_Timer;
    uint16_t Temperature_Low_Threshold;
    uint16_t Temperature_Hihg_Threshold;
} PllSysclkSetup_TypeDef;

typedef struct {
    uint8_t REFA_Receiver_Settings;
    uint8_t REFB_Receiver_Settings;
    uint32_t REFA_R_Divider;
    uint64_t REFA_Input_Period;
    uint32_t REFB_R_Divider;
    uint64_t REFB_Input_Period;
} PllRefSetup_TypeDef;

typedef struct {
    uint64_t Freerun_Tuning_Word;
    uint8_t APLL_M_Divider;
    uint8_t Priority_and_Enable;
    uint8_t Profile_Ref_Source;
    uint8_t ZD_Feedback_Path;
    uint8_t Feedback_Mode;
    uint32_t Loop_BW;
    uint32_t Hitless_FB_Divider;
    uint32_t Buildout_FB_Divider;
    uint32_t Buildout_FB_Fraction;
    uint32_t Buildout_FB_Modulus;
    uint32_t FastLock;
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


void init_PllSysclkSetup(PllSysclkSetup_TypeDef *d);
void init_PllRefSetup(PllRefSetup_TypeDef *d);
void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d);
void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d);
void init_Pll_OutputDrivers_Setup(Pll_OutputDrivers_Setup_TypeDef *d);
void init_Pll_DPLLMode_Setup(Pll_DPLLMode_Setup_TypeDef *d);
void init_Pll_OutputDividers_Setup(Pll_OutputDividers_Setup_TypeDef *d);

#endif // AD9545_SETUP_H
