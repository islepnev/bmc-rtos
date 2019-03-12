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

static const uint8_t sysclk_fb_div = 31;
static const uint32_t ref_r_divide = 209;

static const uint64_t sysclk_Ref_Frequency_milliHz = 38880000ULL * 1000; // milliHertz
//static const uint64_t sysclk_Ref_Frequency_milliHz = 38879880ULL * 1000; // milliHertz
static const double sysclkVcoFreq = sysclk_Ref_Frequency_milliHz * 1e-3 * sysclk_fb_div * 2;

void init_PllSysclkSetup(PllSysclkSetup_TypeDef *d)
{
    d->Sysclk_FB_DIV_Ratio = sysclk_fb_div;
    d->Sysclk_Input = 0x01;
    d->sysclk_Ref_Frequency_milliHz = sysclk_Ref_Frequency_milliHz;
    d->Sysclk_Stability_Timer = 0x32;
    d->Temperature_Low_Threshold = (int16_t)128 * -5;
    d->Temperature_Hihg_Threshold = (int16_t)128 * 70;
    d->AuxDPLL_Bandwidth = 500 * 10; // deciHertz
    d->CompensationValue = 3e-6 * (1ULL<<45); // rel. error * 2^45
    d->TDC_Compensation_Source = 0x11;
    d->DPLL_Compensation_Source = 0x11;
}

void init_PllRefSetup(PllRefSetup_TypeDef *d)
{
    // RefA
    d->REFA_Receiver_Settings = 0x01;
    d->REFA_R_Divider = ref_r_divide;
    d->REFA_Input_Period = 24 * 1000000000ULL; // attoseconds (1e-18 s) units
    d->REFA_Offset_Limit = 10*1000; // ppb units
    d->REFA_Validation_Timer = 10; // milliseconds
    d->REFA_Jitter_Tolerance = 5; // nanoseconds
    // RefB
    d->REFB_Receiver_Settings = 0x01;
    d->REFB_R_Divider = ref_r_divide;
    d->REFB_Input_Period = 24 * 1000000000ULL;
    d->REFB_Offset_Limit = 10*1000; // ppb units
    d->REFB_Validation_Timer = 10; // milliseconds
    d->REFB_Jitter_Tolerance = 5; // nanoseconds
}

enum {
    PROFILE_PRIORITY_DISABLED = 0,
    PROFILE_PRIORITY_HIGH = 0x1 | (0x00 << 1),
    PROFILE_PRIORITY_NORMAL  = 0x1 | (0x0F << 1),
    PROFILE_PRIORITY_LOW  = 0x1 | (0x1F << 1),
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
    PROFILE_INT_ZD_FEEDBACK_OUT0CN = 5,
};
enum {
    PROFILE_INT_ZD_FEEDBACK_OUT1A  = 0,
    PROFILE_INT_ZD_FEEDBACK_OUT1AN = 1,
    PROFILE_INT_ZD_FEEDBACK_OUT1B  = 2,
    PROFILE_INT_ZD_FEEDBACK_OUT1BN = 3,
};
enum {
    PROFILE_EXT_ZD_FEEDBACK_REFA   = 0,
    PROFILE_EXT_ZD_FEEDBACK_REFAA  = 1,
    PROFILE_EXT_ZD_FEEDBACK_REFB   = 2,
    PROFILE_EXT_ZD_FEEDBACK_REFBB  = 3,
};
enum {
    PROFILE_REF_SOURCE_A = 0,
    PROFILE_REF_SOURCE_AA = 1,
    PROFILE_REF_SOURCE_B = 2,
    PROFILE_REF_SOURCE_BB = 3,
    PROFILE_REF_SOURCE_DPLL0 = 4,
    PROFILE_REF_SOURCE_DPLL1 = 5,
    PROFILE_REF_SOURCE_NCO0 = 8,
    PROFILE_REF_SOURCE_NCO1 = 9,
};

enum {
    DPLL_MODE_PROFILE_SELECT_AUTOMATIC = 0,
    DPLL_MODE_PROFILE_SELECT_FALLBACK_PRIORITY = 1,
    DPLL_MODE_PROFILE_SELECT_FALLBACK_HOLDOVER = 2,
    DPLL_MODE_PROFILE_SELECT_MANUAL = 3,
};
static void init_DPLL0_Setup(Pll_DPLL_Setup_TypeDef *d)
{
    const double targetFreq = 312.5e6;
    d->Freerun_Tuning_Word = (1ULL << 48) * (targetFreq / sysclkVcoFreq);
    d->FTW_Offset_Clamp = 0xFFFFFF; // 200000;
    d->APLL_M_Divider = 8;
//    memset(&d->profile[0], 0, sizeof(Pll_DPLL_Profile_TypeDef));
    // Translation Profile 0.0
    d->profile[0].Priority_and_Enable = PROFILE_PRIORITY_NORMAL;
    d->profile[0].Profile_Ref_Source = PROFILE_REF_SOURCE_A;
    d->profile[0].ZD_Feedback_Path = PROFILE_EXT_ZD_FEEDBACK_REFB;
    d->profile[0].Feedback_Mode.b.enable_hitless = 1;
    d->profile[0].Feedback_Mode.b.enable_ext_zd = 1;
    d->profile[0].Loop_BW = 50u * 1000000; // microHertz
    d->profile[0].Hitless_FB_Divider = ref_r_divide;
    d->profile[0].Buildout_FB_Divider = 1567; // 2*R*Q/M
    d->profile[0].Buildout_FB_Fraction = 1;
    d->profile[0].Buildout_FB_Modulus = 2;
    d->profile[0].FastLock = 4; // 4: 100 ms
    // Translation Profile 0.1
//    d->profile[1] = d->profile[0];
//    d->profile[1].Priority_and_Enable = PROFILE_PRIORITY_DISABLED;
//    d->profile[1].Feedback_Mode.b.enable_hitless = 1;
//    d->profile[1].Feedback_Mode.b.enable_ext_zd = 0;
}

static void init_DPLL1_Setup(Pll_DPLL_Setup_TypeDef *d)
{
    const double targetFreq = 325e6;
    d->Freerun_Tuning_Word = (1ULL << 48) * (targetFreq / sysclkVcoFreq);
    d->FTW_Offset_Clamp = 0xFFFFFF; // 200000;
    d->APLL_M_Divider = 10;
    // Translation Profile 1.0
    d->profile[0].Priority_and_Enable = PROFILE_PRIORITY_NORMAL;
    d->profile[0].Profile_Ref_Source = PROFILE_REF_SOURCE_A;
    d->profile[0].ZD_Feedback_Path = PROFILE_INT_ZD_FEEDBACK_OUT1A;
    d->profile[0].Feedback_Mode.b.enable_hitless = 1;
    d->profile[0].Feedback_Mode.b.enable_ext_zd = 0;
    d->profile[0].Loop_BW = 500 * 1000000UL; // microHertz
    d->profile[0].Hitless_FB_Divider = ref_r_divide;
    d->profile[0].Buildout_FB_Divider = 1630; // 2*R*Q/M
    d->profile[0].Buildout_FB_Fraction = 2;
    d->profile[0].Buildout_FB_Modulus = 5;
    d->profile[0].FastLock = 4; // 4: 100 ms
    // Translation Profile 1.1
//    d->profile[1] = d->profile[0];
//    d->profile[1].Priority_and_Enable = PROFILE_PRIORITY_NORMAL;
//    d->profile[1].Feedback_Mode = 0;
//    d->profile[1].Profile_Ref_Source = PROFILE_REF_SOURCE_B;
//    d->profile[1].ZD_Feedback_Path = 0; // Out1A
//    d->profile[1].Feedback_Mode.b.enable_hitless = 1;
//    d->profile[1].Feedback_Mode.b.enable_ext_zd = 0;
}

void init_DPLL_Setup(Pll_DPLL_Setup_TypeDef *d, PllChannel_TypeDef channel)
{
    switch(channel) {
    case DPLL0:
        init_DPLL0_Setup(d);
        break;
    case DPLL1:
        init_DPLL1_Setup(d);
        break;
    }
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
    d->dpll0_mode.b.force_holdover = 0;
    d->dpll0_mode.b.tr_prof_select_mode = DPLL_MODE_PROFILE_SELECT_AUTOMATIC;
    d->dpll0_mode.b.assign_tr_prof = 0;
    d->dpll0_mode.b.enable_step_detect_ref_fault = 0;

    d->dpll1_mode.raw = 0;
    d->dpll1_mode.b.force_freerun = 0;
    d->dpll1_mode.b.force_holdover = 0;
    d->dpll1_mode.b.tr_prof_select_mode = DPLL_MODE_PROFILE_SELECT_AUTOMATIC;
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
    d->Automute_Control_0 = 0; // 0xFC;
    d->Automute_Control_1 = 0;
    d->Distribution_Divider_0_A = 30;
    d->Distribution_Divider_0_B = 30;
    d->Distribution_Divider_0_C = 30;
    d->Distribution_Divider_1_A = 39;
    d->Distribution_Divider_1_B = 39;
}
