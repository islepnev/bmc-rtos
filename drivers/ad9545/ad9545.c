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

#include "ad9545.h"

#include "gpio.h"
#include "bsp_pin_defs.h"
#include "ad9545_setup.h"
#include "ad9545_i2c_hal.h"
#include "ad9545_regs.h"
#include "ad9545_status_regs.h"
#include "ad9545_print.h"
#include "ansi_escape_codes.h"
#include "logbuffer.h"

static const uint8_t AD9545_OPER_CONTROL_DEFAULT = 0x0A; // shutdown RefAA, RefBB

void init_ad9545_setup(ad9545_setup_t *setup)
{
    init_Pll_OutputDrivers_Setup(&setup->out_drivers);
    init_Pll_OutputDividers_Setup(&setup->out_dividers);
    init_DPLL_Setup(&setup->dpll0, DPLL0);
    init_DPLL_Setup(&setup->dpll1, DPLL1);
    init_Pll_DPLLMode_Setup(&setup->dpll_mode);
    init_PllRefSetup(&setup->ref);
    init_PllSysclkSetup(&setup->sysclk);
}

static bool pllIoUpdate(BusInterface *bus)
{
    uint8_t data = 1;
    return ad9545_write1(bus, 0x000F, data);
}

bool ad9545_detect(BusInterface *bus)
{
    bool deviceError = 0;
    bool devicePresent = ad9545_i2c_detect(bus);
    if (devicePresent) {
        uint32_t data = 0;
        devicePresent = ad9545_read3(bus, AD9545_REG_VENDOR_ID, &data) &&
                        (data == AD9545_VENDOR_ID);
    }
    if (devicePresent) {
        uint32_t test = 0xFF0055AA;
        for (int i=0; i<100; i++) {
            // scratchpad test
            test = ~test;
            uint32_t data = 0;
            if (! ad9545_write4(bus, 0x0020, test) ||
                ! ad9545_read4(bus, 0x0020, &data) ||
                (data != test)) {
                deviceError = 1;
                break;
            }
        }
    }
    if (devicePresent) {
        if (deviceError) {
            log_put(LOG_ERR, "PLL I2C data error");
        } else {
            log_put(LOG_INFO, "PLL AD9545 present");
        }
    }
    return devicePresent && !deviceError;
}

bool ad9545_software_reset(BusInterface *bus)
{
    return ad9545_write1(bus, 0x0000, 0x81) &&
           ad9545_write1(bus, 0x0000, 0);
}

bool ad9545_setup_sysclk(BusInterface *bus, const PllSysclkSetup_TypeDef *sysclkSetup)
{
    return ad9545_write1(bus, AD9545_REG1_Sysclk_FB_DIV_Ratio, sysclkSetup->Sysclk_FB_DIV_Ratio) &&
           ad9545_write1(bus, AD9545_REG1_Sysclk_Input, sysclkSetup->Sysclk_Input) &&
           ad9545_write5(bus, AD9545_REG5_Sysclk_Ref_Frequency, sysclkSetup->sysclk_Ref_Frequency_milliHz) &&
           ad9545_write3(bus, AD9545_REG3_Sysclk_Stability_Timer, sysclkSetup->Sysclk_Stability_Timer) &&
           ad9545_write1(bus, AD9545_REG1_0280, sysclkSetup->TDC_Compensation_Source) &&
           ad9545_write1(bus, AD9545_REG1_0282, sysclkSetup->DPLL_Compensation_Source) &&
           ad9545_write2(bus, AD9545_REG2_0285, sysclkSetup->AuxDPLL_Bandwidth) &&
           ad9545_write5(bus, AD9545_REG5_0289, sysclkSetup->CompensationValue) &&
           ad9545_write2(bus, 0x2903, sysclkSetup->Temperature_Low_Threshold) &&
           ad9545_write2(bus, 0x2905, sysclkSetup->Temperature_High_Threshold) &&
           pllIoUpdate(bus);
}

static bool pllSetupOutputDrivers(BusInterface *bus, const Pll_OutputDrivers_Setup_TypeDef *setup)
{
    return ad9545_write1(bus, AD9545_REG1_10D7, setup->Driver_Config.raw) &&
           ad9545_write1(bus, AD9545_REG1_10D8, setup->Driver_Config.raw) &&
           ad9545_write1(bus, AD9545_REG1_10D9, setup->Driver_Config.raw) &&
           ad9545_write1(bus, AD9545_REG1_14D7, setup->Driver_Config.raw) &&
           ad9545_write1(bus, AD9545_REG1_14D8, setup->Driver_Config.raw);
}

static bool pllSetupDistribution0WithUpdate(BusInterface *bus, const Pll_OutputDividers_Setup_TypeDef *setup)
{
    // channel 0
    if (! (ad9545_write1(bus, AD9545_REG1_10DA, setup->Secondary_Clock_Path_0) &&
          ad9545_write1(bus, AD9545_REG1_10DC, setup->Automute_Control_0) &&
          ad9545_write1(bus, AD9545_REG1_1100, setup->Distribution_Divider_0_A) &&
          ad9545_write1(bus, AD9545_REG1_1112, setup->Distribution_Divider_0_B) &&
          ad9545_write1(bus, AD9545_REG1_1124, setup->Distribution_Divider_0_C) &&
          pllIoUpdate(bus)))
        return false;
    //    uint8_t Sync_Control_0 = 0x5; // 0x05;
    //    if (! ad9545_write1(bus, AD9545_REG1_10DB, Sync_Control_0)))
    //        return false;

    // When using reference synchronization in conjunction with
    // autosync mode (that is, autosync mode = 1, 2, or 3), be sure to
    // program enable DPLLx reference sync = 1 and assert the I/O_
    // UPDATE bit prior to programming autosync mode = 1, 2, or 3.
    Sync_Control_REG_Type Sync_Control_0;
    Sync_Control_0.raw = 0;
    Sync_Control_0.b.enable_ref_sync = setup->enable_ref_sync_0;
    if (! (ad9545_write1(bus, AD9545_REG1_10DB, Sync_Control_0.raw) &&
          pllIoUpdate(bus)))
        return false;
    Sync_Control_0.b.autosync_mode = setup->autosync_mode_0;
    if (! (ad9545_write1(bus, AD9545_REG1_10DB, Sync_Control_0.raw) &&
          pllIoUpdate(bus)))
        return false;
    return true;
}

static bool pllSetupDistribution1WithUpdate(BusInterface *bus, const Pll_OutputDividers_Setup_TypeDef *setup)
{
    // channel 1
    if (! (ad9545_write1(bus, AD9545_REG1_14DA, setup->Secondary_Clock_Path_1) &&
          ad9545_write1(bus, AD9545_REG1_14DC, setup->Automute_Control_1) &&
          ad9545_write1(bus, AD9545_REG1_1500, setup->Distribution_Divider_1_A) &&
          ad9545_write1(bus, AD9545_REG1_1512, setup->Distribution_Divider_1_B) &&
          pllIoUpdate(bus)))
        return false;

    // When using reference synchronization in conjunction with
    // autosync mode (that is, autosync mode = 1, 2, or 3), be sure to
    // program enable DPLLx reference sync = 1 and assert the I/O_
    // UPDATE bit prior to programming autosync mode = 1, 2, or 3.
    Sync_Control_REG_Type Sync_Control_1;
    Sync_Control_1.raw = 0;
    Sync_Control_1.b.enable_ref_sync = setup->enable_ref_sync_1;
    if (! (ad9545_write1(bus, AD9545_REG1_14DB, Sync_Control_1.raw) &&
          pllIoUpdate(bus)))
        return false;
    Sync_Control_1.b.autosync_mode = setup->autosync_mode_1;
    if (! (ad9545_write1(bus, AD9545_REG1_14DB, Sync_Control_1.raw) &&
          pllIoUpdate(bus)))
        return false;

    return true;
}

static bool pllSetupDistributionWithUpdate(BusInterface *bus, const Pll_OutputDividers_Setup_TypeDef *setup)
{
    return pllSetupDistribution0WithUpdate(bus, setup) &&
           pllSetupDistribution1WithUpdate(bus, setup);
}

static bool pllSetupRef(BusInterface *bus, const PllRefSetup_TypeDef *refSetup)
{
    uint8_t OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT;

    return ad9545_write1(bus, AD9545_REG1_0300, refSetup->REFA_Receiver_Settings) &&
           ad9545_write1(bus, AD9545_REG1_0304, refSetup->REFB_Receiver_Settings) &&

           // REFERENCE INPUT A (REFA) REGISTERS—REGISTER 0x0400 TO REGISTER 0x0414
           ad9545_write4(bus, AD9545_REG4_0400, refSetup->REFA_R_Divider) &&
           ad9545_write8(bus, AD9545_REG8_0404, refSetup->REFA_Input_Period) &&
           ad9545_write3(bus, AD9545_REG3_040C, refSetup->REFA_Offset_Limit) &&
           ad9545_write3(bus, AD9545_REG3_0410, refSetup->REFA_Validation_Timer) &&
           ad9545_write2(bus, AD9545_REG2_0413, refSetup->REFA_Jitter_Tolerance) &&

           // REFERENCE INPUT B (REFB) REGISTERS—REGISTER 0x0440 TO REGISTER 0x0454
           ad9545_write4(bus, AD9545_REG4_0440, refSetup->REFB_R_Divider) &&
           ad9545_write8(bus, AD9545_REG8_0444, refSetup->REFB_Input_Period) &&
           ad9545_write3(bus, AD9545_REG3_044C, refSetup->REFB_Offset_Limit) &&
           ad9545_write2(bus, AD9545_REG2_0450, refSetup->REFB_Validation_Timer) &&
           ad9545_write2(bus, AD9545_REG2_0453, refSetup->REFB_Jitter_Tolerance) &&

           ad9545_write1(bus, AD9545_REG1_2000, OpControlGlobal);
}

static bool pllWriteProfile(BusInterface *bus, PllChannel_TypeDef channel, int profileIndex, Pll_DPLL_Profile_TypeDef profile)
{
    uint16_t reg_offset = (channel == DPLL0) ? AD9545_REG1_1200 : AD9545_REG1_1600;

    reg_offset += profileIndex * 0x20;

    // DPLL TRANSLATION PROFILE REGISTERS
    return ad9545_write1(bus, reg_offset + 0x0, profile.Priority_and_Enable) &&
           ad9545_write1(bus, reg_offset + 0x1, profile.Profile_Ref_Source) &&
           ad9545_write1(bus, reg_offset + 0x2, profile.ZD_Feedback_Path) &&
           ad9545_write1(bus, reg_offset + 0x3, profile.Feedback_Mode.raw) &&
           ad9545_write4(bus, reg_offset + 0x4, profile.Loop_BW) &&
           ad9545_write4(bus, reg_offset + 0x8, profile.Hitless_FB_Divider) &&
           ad9545_write4(bus, reg_offset + 0xC, profile.Buildout_FB_Divider) &&
           ad9545_write3(bus, reg_offset + 0x10, profile.Buildout_FB_Fraction) &&
           ad9545_write3(bus, reg_offset + 0x13, profile.Buildout_FB_Modulus) &&
           ad9545_write3(bus, reg_offset + 0x17, profile.FastLock);
}

static bool pllSetupDPLLChannel(BusInterface *bus, const Pll_DPLL_Setup_TypeDef *dpll, PllChannel_TypeDef channel)
{
    uint16_t reg_offset = (channel == DPLL0) ? 0x0 : 0x400;

    return
        // DPLL CHANNEL REGISTERS
        ad9545_write6(bus, reg_offset + 0x1000, dpll->Freerun_Tuning_Word) &&
        ad9545_write3(bus, reg_offset + 0x1006, dpll->FTW_Offset_Clamp) &&

        // APLL CHANNEL REGISTERS
        ad9545_write1(bus, reg_offset + 0x1081, dpll->APLL_M_Divider) &&

        pllWriteProfile(bus, channel, 0, dpll->profile[0]) &&
        pllWriteProfile(bus, channel, 1, dpll->profile[1]);
}

static bool pllSetupDPLL(BusInterface *bus, const ad9545_setup_t *d)
{
    return pllSetupDPLLChannel(bus, &d->dpll0, DPLL0) &&
           pllSetupDPLLChannel(bus, &d->dpll1, DPLL1);
}

static bool pllSetupDPLLMode(BusInterface *bus, const Pll_DPLLMode_Setup_TypeDef *dpll_mode)
{
    return ad9545_write1(bus, AD9545_REG1_2105, dpll_mode->dpll0_mode.raw) &&
           ad9545_write1(bus, AD9545_REG1_2205, dpll_mode->dpll1_mode.raw);
}

bool ad9545_calibrate_sysclk(BusInterface *bus)
{
    return ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x04) &&
           pllIoUpdate(bus) &&
           ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x04) &&
           pllIoUpdate(bus);
}

static bool pllCalibrateAll(BusInterface *bus)
{
    return ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x02) &&
           pllIoUpdate(bus) &&
           ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x02) &&
           pllIoUpdate(bus);
}

static bool pllSyncAllDistDividers(BusInterface *bus)
{
    return ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x08) &&
           pllIoUpdate(bus) &&
           ad9545_write1(bus, AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x08) &&
           pllIoUpdate(bus);
}

static bool pllReadRefStatus(BusInterface *bus, AD9545_Status *status)
{
    uint8_t refa;
    if (! ad9545_read1(bus, AD9545_REG1_3005, &refa))
        return false;
    status->ref[0].raw = refa;

    uint8_t refb;
    if (! ad9545_read1(bus, AD9545_REG1_3005, &refb))
        return false;
    status->ref[2].raw = refb;

    return true;
}

static bool pllReadDPLLChannelStatus(BusInterface *bus, DPLL_Status *dpll_status, PllChannel_TypeDef channel)
{
    uint16_t reg_offset = (channel == DPLL0) ? 0x0 : 0x100;

    return ad9545_read1(bus, AD9545_REG1_3009 + 1 * channel, &dpll_status->act_profile.raw) &&
           ad9545_read1(bus, AD9545_REG1_3100 + reg_offset, &dpll_status->lock_status.raw) &&
           ad9545_read1(bus, AD9545_REG1_3101 + reg_offset, &dpll_status->operation.raw) &&
           ad9545_read1(bus, AD9545_REG1_3102 + reg_offset, &dpll_status->state.raw) &&
           ad9545_read2(bus, AD9545_REG2_3109 + reg_offset, &dpll_status->pld_tub) &&
           ad9545_read2(bus, AD9545_REG2_310B + reg_offset, &dpll_status->fld_tub) &&
           ad9545_read6(bus, AD9545_REG6_3103 + reg_offset, &dpll_status->ftw_history) &&
           ad9545_read1(bus, AD9545_REG1_310D + reg_offset, &dpll_status->phase_slew) &&
           ad9545_read1(bus, AD9545_REG1_310E + reg_offset, &dpll_status->phase_control_error);
}

bool ad9545_read_status(BusInterface *bus, AD9545_Status *status)
{
    return pllIoUpdate(bus) &&
           ad9545_read1(bus, AD9545_LIVE_REG1_3000, &status->eeprom.raw) &&
           ad9545_read1(bus, AD9545_LIVE_REG1_3001, &status->sysclk.raw) &&
           ad9545_read1(bus, AD9545_REG1_3002, &status->misc.raw) &&
           ad9545_read2(bus, AD9545_REG2_INT_THERM, (uint16_t *)&status->internal_temp) &&
           pllReadRefStatus(bus, status) &&
           pllReadDPLLChannelStatus(bus, &status->dpll[0], DPLL0) &&
           pllReadDPLLChannelStatus(bus, &status->dpll[1], DPLL1);
}

bool ad9545_read_sysclk_status(BusInterface *bus, AD9545_Status *status)
{
    return ad9545_read1(bus, AD9545_LIVE_REG1_3001, &status->sysclk.raw);
}

bool ad9545_setup(BusInterface *bus, const ad9545_setup_t *setup)
{
    return pllSetupOutputDrivers(bus, &setup->out_drivers) &&
           pllSetupDPLL(bus, setup) &&
           pllSetupDPLLMode(bus, &setup->dpll_mode) &&
           pllSetupRef(bus, &setup->ref) &&
           pllSetupDistributionWithUpdate(bus, &setup->out_dividers) &&
           pllCalibrateAll(bus) &&
           pllSyncAllDistDividers(bus);
}

void ad9545_reset(BusInterface *bus)
{
    // toggle reset_b pin
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
}

bool ad9545_gpio_test(BusInterface *bus)
{
    GPIO_PinState pin_resetb = HAL_GPIO_ReadPin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin);
    GPIO_PinState pin_m3 = HAL_GPIO_ReadPin(PLL_M3_GPIO_Port, PLL_M3_Pin);
    GPIO_PinState pin_m4 = HAL_GPIO_ReadPin(PLL_M4_GPIO_Port, PLL_M4_Pin);
    GPIO_PinState pin_m5 = HAL_GPIO_ReadPin(PLL_M5_GPIO_Port, PLL_M5_Pin);
    GPIO_PinState pin_m6 = HAL_GPIO_ReadPin(PLL_M6_GPIO_Port, PLL_M6_Pin);
    if ( 1
        && (GPIO_PIN_SET   == pin_resetb)
        && (GPIO_PIN_RESET == pin_m3)
        && (GPIO_PIN_SET   == pin_m4)
        && (GPIO_PIN_RESET == pin_m5)
        && (GPIO_PIN_SET   == pin_m6)
        )
        return true;
    else {
        log_printf(LOG_NOTICE, "PLL GPIO: resetb=%u, m3=%u, m4=%u, m5=%u, m6=%u", pin_resetb, pin_m3, pin_m4, pin_m5, pin_m6);
        return false;
    }
}

// AD9545
// M0, M1, M2 do not have internal resistors
void ad9545_gpio_init(BusInterface *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // pull-up on PCB, internal 100 kΩ pull-up resistor
    GPIO_InitStruct.Pin = PLL_RESET_B_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_RESET_B_GPIO_Port, &GPIO_InitStruct);

    // input
    GPIO_InitStruct.Pin = PLL_M0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PLL_M0_GPIO_Port, &GPIO_InitStruct);

    // M3=0 - do not load eeprom.
    // internal 100 kΩ pull-down, disable pin
    GPIO_InitStruct.Pin = PLL_M3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(PLL_M3_GPIO_Port, &GPIO_InitStruct);

    // M4=1 - I2C mode
    // pull-up on PCB, internal 100 kΩ pull-down
    GPIO_InitStruct.Pin = PLL_M4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_M4_GPIO_Port, &GPIO_InitStruct);

    // M5=0 - I2C address offset
    // pull-down on PCB
    GPIO_InitStruct.Pin = PLL_M5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(PLL_M5_GPIO_Port, &GPIO_InitStruct);

    // M6=1 - I2C address offset, internal 10 kΩ pull-up resistor
    // pull-up on PCB
    GPIO_InitStruct.Pin = PLL_M6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_M6_GPIO_Port, &GPIO_InitStruct);
}
