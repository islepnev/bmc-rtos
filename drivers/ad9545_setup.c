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

#include "ad9545_setup.h"

static const uint32_t ref_r_divide = 209;

//static const uint64_t sysclk_Ref_Frequency = 38880000ULL * 1000; // milliHertz
static const uint64_t sysclk_Ref_Frequency_milliHz = 38879880ULL * 1000; // milliHertz

void init_PllSysclkSetup(PllSysclkSetup_TypeDef *d)
{
    d->Sysclk_FB_DIV_Ratio = 0x1F;
    d->Sysclk_Input = 0x01;
    d->sysclk_Ref_Frequency_milliHz = 38879880ULL * 1000; // milliHertz
    d->Sysclk_Stability_Timer = 0x32;
    d->Temperature_Low_Threshold = (int16_t)128 * -5;
    d->Temperature_Hihg_Threshold = (int16_t)128 * 70;
}

void init_PllRefSetup(PllRefSetup_TypeDef *d)
{
    d->REFA_Receiver_Settings = 0x01;
    d->REFB_Receiver_Settings = 0x01;
    d->REFA_R_Divider = ref_r_divide;
    d->REFA_Input_Period = 24000000000ULL; // * 1e-18 sec, 59682F000ULL;
    d->REFB_R_Divider = ref_r_divide;
    d->REFB_Input_Period = 24000000000ULL;
}

void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d)
{
    const double targetFreq = 312.5e6;
    const double sysclkVco = sysclk_Ref_Frequency_milliHz * 1e-3 * 31.0 * 2;
    d->Freerun_Tuning_Word = (1ULL << 48) * (targetFreq / sysclkVco);
    d->APLL_M_Divider = 8;
    d->Priority_and_Enable = 0x01;
    d->Profile_Ref_Source = 0x0; // 0: RefA
    d->ZD_Feedback_Path = 0; // Out0A
    d->Feedback_Mode = 1; // 0x03;
    d->Loop_BW = 50u * 1000000; // microHertz
    d->Hitless_FB_Divider = ref_r_divide;
    d->Buildout_FB_Divider = ref_r_divide;
    d->Buildout_FB_Fraction = 0;
    d->Buildout_FB_Modulus = 0;
    d->FastLock = 4; // 4: 100 ms
}

void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d)
{
    const double targetFreq = 325e6;
    const double sysclkVco = sysclk_Ref_Frequency_milliHz * 1e-3 * 31.0 * 2;
    d->Freerun_Tuning_Word = (1ULL << 48) * (targetFreq / sysclkVco);
    d->APLL_M_Divider = 10;
    d->Priority_and_Enable = 0x01;
    d->Profile_Ref_Source = 0x0; // 0: RefA
    d->ZD_Feedback_Path = 0; // Out0A
    d->Feedback_Mode = 1; // 0x03;
    d->Loop_BW = 50u * 1000000; // microHertz
    d->Hitless_FB_Divider = ref_r_divide;
    d->Buildout_FB_Divider = ref_r_divide;
    d->Buildout_FB_Fraction = 0;
    d->Buildout_FB_Modulus = 0;
    d->FastLock = 4; // 4: 100 ms
}

void init_Pll_OutputDrivers_Setup(Pll_OutputDrivers_Setup_TypeDef *d)
{
    d->Driver_Config.raw = 0;
    d->Driver_Config.b.enable_hcsl = 1;
    d->Driver_Config.b.driver_current = 2;
    d->Driver_Config.b.driver_mode = 0;
}

void init_Pll_DPLLMode_Setup(Pll_DPLLMode_Setup_TypeDef *d)
{
    d->dpll0_mode.raw = 0;
    d->dpll0_mode.b.force_freerun = 0;
    d->dpll0_mode.b.force_holdover = 1;
    d->dpll0_mode.b.tr_prof_select_mode = 2;
    d->dpll0_mode.b.assign_tr_prof = 0;
    d->dpll0_mode.b.enable_step_detect_ref_fault = 0;

    d->dpll1_mode.raw = 0;
    d->dpll1_mode.b.force_freerun = 0;
    d->dpll1_mode.b.force_holdover = 0;
    d->dpll1_mode.b.tr_prof_select_mode = 2;
    d->dpll1_mode.b.assign_tr_prof = 0;
    d->dpll1_mode.b.enable_step_detect_ref_fault = 0;
}

void init_Pll_OutputDividers_Setup(Pll_OutputDividers_Setup_TypeDef *d)
{
    d->enable_ref_sync_0 = 1;
    d->enable_ref_sync_1 = 1;
    d->autosync_mode_0 = AutosyncMode_Immediate;
    d->autosync_mode_1 = AutosyncMode_Immediate;
    d->Secondary_Clock_Path_0 = 0x0; // 0x0E;
    d->Secondary_Clock_Path_1 = 0x0; // 0x06;
    d->Automute_Control_0 = 0; // 0xFC
    d->Automute_Control_1 = 0; // 0xFC
    d->Distribution_Divider_0_A = 30;
    d->Distribution_Divider_0_B = 30;
    d->Distribution_Divider_0_C = 30;
    d->Distribution_Divider_1_A = 39;
    d->Distribution_Divider_1_B = 39;
}
