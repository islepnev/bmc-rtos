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

#include "ad9545.h"

#include "stm32f7xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bsp_ad9545.h"
#include "ad9545_i2c_hal.h"
#include "ad9545_regs.h"
#include "ad9545_status_regs.h"
#include "ad9545_print.h"
#include "ansi_escape_codes.h"
#include "logbuffer.h"

static void DEBUG_PRINT_RET(const char *func, int ret)
{
    log_printf(LOG_ERR, "%s failed: return code %d, I2C error 0x%08lX",
           func, ret, hi2c_ad9545->ErrorCode);
}

static const uint8_t AD9545_OPER_CONTROL_DEFAULT = 0x0A; // shutdown RefAA, RefBB

void init_ad9545_setup(ad9545_setup_t *setup)
{
    init_Pll_OutputDrivers_Setup(&setup->out_drivers);
    init_Pll_OutputDividers_Setup(&setup->out_dividers);
    init_DPLL_Setup(&setup->dpll0, 0);
    init_DPLL_Setup(&setup->dpll1, 1);
    init_Pll_DPLLMode_Setup(&setup->dpll_mode);
    init_PllRefSetup(&setup->ref);
    init_PllSysclkSetup(&setup->sysclk);
}

static HAL_StatusTypeDef pllIoUpdate(void)
{
    uint8_t data = 1;
    HAL_StatusTypeDef ret = ad9545_write1(0x000F, data);
    if (ret != HAL_OK)
        goto err;
//    osDelay(1);
    return ret;
err:
//    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static HAL_StatusTypeDef pllRegisterPulseBit_unused(uint16_t address, uint8_t bitmask)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    uint8_t data = 0;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_read1(address, &data)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(address, data | bitmask)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(address, data & ~bitmask)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

bool ad9545_detect(void)
{
    int devicePresent = 0;
    int deviceError = 0;
    HAL_StatusTypeDef ret = ad9545_i2c_detect();
    devicePresent = (HAL_OK == ret);
    if (devicePresent) {
        uint32_t data = 0;
        ad9545_read3(AD9545_REG_VENDOR_ID, &data);
        devicePresent = (data == AD9545_VENDOR_ID);
    }
    if (devicePresent) {
        uint32_t test = 0xFF0055AA;
        for (int i=0; i<100; i++) {
            // scratchpad test
            test = ~test;
            ad9545_write4(0x0020, test);
            uint32_t data = 0;
            ad9545_read4(0x0020, &data);
            if (data != test) {
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

bool ad9545_software_reset(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    ret = ad9545_write1(0x0000, 0x81);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_write1(0x0000, 0);
    if (ret != HAL_OK)
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

bool ad9545_setup_sysclk(const PllSysclkSetup_TypeDef *sysclkSetup)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_FB_DIV_Ratio, sysclkSetup->Sysclk_FB_DIV_Ratio)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_Input, sysclkSetup->Sysclk_Input)))
        goto err;
    if (HAL_OK != (ret = ad9545_write5(AD9545_REG5_Sysclk_Ref_Frequency, sysclkSetup->sysclk_Ref_Frequency_milliHz)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_Sysclk_Stability_Timer, sysclkSetup->Sysclk_Stability_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0280, sysclkSetup->TDC_Compensation_Source)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0282, sysclkSetup->DPLL_Compensation_Source)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0285, sysclkSetup->AuxDPLL_Bandwidth)))
        goto err;
    if (HAL_OK != (ret = ad9545_write5(AD9545_REG5_0289, sysclkSetup->CompensationValue)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(0x2903, sysclkSetup->Temperature_Low_Threshold)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(0x2905, sysclkSetup->Temperature_High_Threshold)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static HAL_StatusTypeDef pllCalibrateApll_unused(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    // calibrate APLL 0 (requires IO Update, not autoclearing)
    uint8_t OpControlChannel0 = 0x02; // calibrate
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2100, OpControlChannel0)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    OpControlChannel0 = 0; // clear calibrate bit
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2100, OpControlChannel0)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    // calibrate APLL 1 (requires IO Update, not autoclearing)
    uint8_t OpControlChannel1 = 0x02; // calibrate
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2200, OpControlChannel1)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    OpControlChannel1 = 0; // clear calibrate bit
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2200, OpControlChannel1)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static HAL_StatusTypeDef pllResetOutputDividers_unused(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2102, 0x1)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2103, 0x1)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2104, 0x1)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2202, 0x1)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2203, 0x1)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2102, 0x0)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2103, 0x0)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2104, 0x0)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2202, 0x0)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2203, 0x0)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static bool pllSetupOutputDrivers(const Pll_OutputDrivers_Setup_TypeDef *setup)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D7, setup->Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D8, setup->Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D9, setup->Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14D7, setup->Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14D8, setup->Driver_Config.raw)))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllClearAutomute_unused(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2107, 0x10)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2207, 0x10)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2107, 0x00)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2207, 0x00)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllSetupDistributionWithUpdate(const Pll_OutputDividers_Setup_TypeDef *setup)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    // channel 0
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DA, setup->Secondary_Clock_Path_0)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DC, setup->Automute_Control_0)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1100, setup->Distribution_Divider_0_A)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1112, setup->Distribution_Divider_0_B)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1124, setup->Distribution_Divider_0_C)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    //    uint8_t Sync_Control_0 = 0x5; // 0x05;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DB, Sync_Control_0)))
//        goto err;

    // When using reference synchronization in conjunction with
    // autosync mode (that is, autosync mode = 1, 2, or 3), be sure to
    // program enable DPLLx reference sync = 1 and assert the I/O_
    // UPDATE bit prior to programming autosync mode = 1, 2, or 3.
    Sync_Control_REG_Type Sync_Control_0;
    Sync_Control_0.raw = 0;
    Sync_Control_0.b.enable_ref_sync = setup->enable_ref_sync_0;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DB, Sync_Control_0.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    Sync_Control_0.b.autosync_mode = setup->autosync_mode_0;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DB, Sync_Control_0.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    // channel 1
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DA, setup->Secondary_Clock_Path_1)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DC, setup->Automute_Control_1)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1500, setup->Distribution_Divider_1_A)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1512, setup->Distribution_Divider_1_B)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    // When using reference synchronization in conjunction with
    // autosync mode (that is, autosync mode = 1, 2, or 3), be sure to
    // program enable DPLLx reference sync = 1 and assert the I/O_
    // UPDATE bit prior to programming autosync mode = 1, 2, or 3.
    Sync_Control_REG_Type Sync_Control_1;
    Sync_Control_1.raw = 0;
    Sync_Control_1.b.enable_ref_sync = setup->enable_ref_sync_1;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DB, Sync_Control_1.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    Sync_Control_1.b.autosync_mode = setup->autosync_mode_1;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DB, Sync_Control_1.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllSetupRef(const PllRefSetup_TypeDef *refSetup)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0300, refSetup->REFA_Receiver_Settings)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0304, refSetup->REFB_Receiver_Settings)))
        goto err;

    // REFERENCE INPUT A (REFA) REGISTERS—REGISTER 0x0400 TO REGISTER 0x0414

    if (HAL_OK != (ret = ad9545_write4(AD9545_REG4_0400, refSetup->REFA_R_Divider)))
        goto err;
    if (HAL_OK != (ret = ad9545_write8(AD9545_REG8_0404, refSetup->REFA_Input_Period)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_040C, refSetup->REFA_Offset_Limit)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_0410, refSetup->REFA_Validation_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0413, refSetup->REFA_Jitter_Tolerance)))
        goto err;

    // REFERENCE INPUT B (REFB) REGISTERS—REGISTER 0x0440 TO REGISTER 0x0454

    if (HAL_OK != (ret = ad9545_write4(AD9545_REG4_0440, refSetup->REFB_R_Divider)))
        goto err;
    if (HAL_OK != (ret = ad9545_write8(AD9545_REG8_0444, refSetup->REFB_Input_Period)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_044C, refSetup->REFB_Offset_Limit)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0450, refSetup->REFB_Validation_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0453, refSetup->REFB_Jitter_Tolerance)))
        goto err;

    uint8_t OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, OpControlGlobal)))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static HAL_StatusTypeDef pllWriteProfile(PllChannel_TypeDef channel, int profileIndex, Pll_DPLL_Profile_TypeDef profile)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    uint16_t reg_offset = AD9545_REG1_1200;
    switch(channel) {
    case DPLL0:
        reg_offset += 0;
        break;
    case DPLL1:
        reg_offset += 0x400;
        break;
    }
    reg_offset += profileIndex * 0x20;

    // DPLL TRANSLATION PROFILE REGISTERS
    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x0, profile.Priority_and_Enable)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x1, profile.Profile_Ref_Source)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x2, profile.ZD_Feedback_Path)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x3, profile.Feedback_Mode.raw)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + 0x4, profile.Loop_BW)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + 0x8, profile.Hitless_FB_Divider)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + 0xC, profile.Buildout_FB_Divider)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + 0x10, profile.Buildout_FB_Fraction)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + 0x13, profile.Buildout_FB_Modulus)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + 0x17, profile.FastLock)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static bool pllSetupDPLLChannel(const Pll_DPLL_Setup_TypeDef *dpll, PllChannel_TypeDef channel)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint16_t reg_offset = 0x0;
    switch(channel) {
    case DPLL0:
        break;
    case DPLL1:
        reg_offset = 0x400;
        break;
    }

    // DPLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write6(reg_offset + 0x1000, dpll->Freerun_Tuning_Word)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(reg_offset + 0x1006, dpll->FTW_Offset_Clamp)))
        goto err;

    // APLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x1081, dpll->APLL_M_Divider)))
        goto err;

    for (int i=0; i<2; i++)
        if (HAL_OK != (ret = pllWriteProfile(channel, i, dpll->profile[i])))
            goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllSetupDPLL(const ad9545_setup_t *d)
{
    if (!pllSetupDPLLChannel(&d->dpll0, DPLL0))
        return false;
    if (!pllSetupDPLLChannel(&d->dpll1, DPLL1))
        return false;
    return true;
}

static bool pllSetupDPLLMode(const Pll_DPLLMode_Setup_TypeDef *dpll_mode)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2105, dpll_mode->dpll0_mode.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2205, dpll_mode->dpll1_mode.raw)))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

bool ad9545_calibrate_sysclk(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x04)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x04)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllCalibrateAll(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x02)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x02)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static bool pllSyncAllDistDividers(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x08)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x08)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate()))
        goto err;

    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static HAL_StatusTypeDef pllReadRefStatus(AD9545_Status *status)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint8_t refa;
    ret = ad9545_read1(AD9545_REG1_3005, &refa);
    if (ret != HAL_OK)
        goto err;
    status->ref[0].raw = refa;

    uint8_t refb;
    ret = ad9545_read1(AD9545_REG1_3005, &refb);
    if (ret != HAL_OK)
        goto err;
    status->ref[2].raw = refb;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static HAL_StatusTypeDef pllReadDPLLChannelStatus(DPLL_Status *dpll_status, PllChannel_TypeDef channel)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    uint16_t reg_offset = 0x0;
    switch(channel) {
    case DPLL0:
        break;
    case DPLL1:
        reg_offset = 0x100;
        break;
    }

    ret = ad9545_read1(AD9545_REG1_3009 + 1 * channel, &dpll_status->act_profile.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_3100 + reg_offset, &dpll_status->lock_status.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_3101 + reg_offset, &dpll_status->operation.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_3102 + reg_offset, &dpll_status->state.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read2(AD9545_REG2_3109 + reg_offset, &dpll_status->pld_tub);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read2(AD9545_REG2_310B + reg_offset, &dpll_status->fld_tub);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read6(AD9545_REG6_3103 + reg_offset, &dpll_status->ftw_history);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_310D + reg_offset, &dpll_status->phase_slew);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_310E + reg_offset, &dpll_status->phase_control_error);
    if (ret != HAL_OK)
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

bool ad9545_read_status(AD9545_Status *status)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    ret = pllIoUpdate();
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_LIVE_REG1_3000, &status->eeprom.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_LIVE_REG1_3001, &status->sysclk.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_3002, &status->misc.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read2(AD9545_REG2_INT_THERM, (uint16_t *)&status->internal_temp);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadRefStatus(status);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadDPLLChannelStatus(&status->dpll[0], DPLL0);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadDPLLChannelStatus(&status->dpll[1], DPLL1);
    if (ret != HAL_OK)
        goto err;

//    // read
//    for (int i=0x3000; i<=0x300A; i++) {
//        uint8_t data;
//        ret = ad9545_read1(i, &data);
//        if (ret != HAL_OK)
//            goto err;
//        printf("[%04X] = %02X\n", i, data);
//    }

    //    osDelay(1000);
    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

bool ad9545_read_sysclk_status(AD9545_Status *status)
{
    // read sysclk status
    HAL_StatusTypeDef ret = ad9545_read1(AD9545_LIVE_REG1_3001, &status->sysclk.raw);
    if (ret != HAL_OK)
        goto err;
    return true;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return false;
}

static HAL_StatusTypeDef pllReadAllRegisters_unused(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    typedef struct {
        uint16_t first;
        uint16_t last;
    } region_t;
    enum {size = 41};
    region_t regs[size] = {
        {0x0000,0x0010},
        {0x0020,0x0023},
        {0x0100,0x011A},
        {0x0200,0x0209},
        {0x0280,0x029C},
        {0x0300,0x0307},
        {0x0400,0x0414}, // RefA
        {0x0420,0x0434}, // RefAA
        {0x0440,0x0454}, // RefB
        {0x0460,0x0474}, // RefBB
        {0x0800,0x0811},
        {0x0820,0x0831},
        {0x0840,0x0851},
        {0x0860,0x0871},
        {0x0880,0x0891},
        {0x08A0,0x08B1},
        {0x08C0,0x08D1},
        {0x08E0,0x08F1},
        {0x0C00,0x0C17},
        {0x1000,0x102B},
        {0x1080,0x1083},
        {0x10C0,0x10DC},
        {0x1100,0x1135},
        {0x1200,0x12B7},
        {0x1400,0x142B},
        {0x1480,0x1483},
        {0x14C0,0x14DC},
        {0x1500,0x1523},
        {0x1600,0x16B7},
        {0x2000,0x2014},
        {0x2100,0x2107},
        {0x2200,0x2207},
        {0x2800,0x281E},
        {0x2840,0x285E},
        {0x2900,0x2906},
        {0x2A00,0x2A17},
        {0x2E00,0x2E1E},
        {0x3000,0x3019},
        {0x3100,0x310E},
        {0x3200,0x320E},
        {0x3A00,0x3A3B},
    };
    for (int n=0; n<size; n++) {
        uint16_t first = regs[n].first;
        uint16_t last = regs[n].last;
        for (int i=first; i<=last; i++) {
            uint8_t data = 0;
            ret = ad9545_read1(i, &data);
            if (ret != HAL_OK)
                goto err;
//            printf("0x%04X,0x%02X\n", i, data);
        }
    }
    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

void ad9545_reset_i2c(void)
{
    __HAL_I2C_DISABLE(hi2c_ad9545);
    __HAL_I2C_ENABLE(hi2c_ad9545);
}

bool ad9545_setup(const ad9545_setup_t *setup)
{
    if (!pllSetupOutputDrivers(&setup->out_drivers))
        return false;
    if (!pllSetupDPLL(setup))
        return false;
    if (!pllSetupDPLLMode(&setup->dpll_mode))
        return false;
    if (!pllSetupRef(&setup->ref))
        return false;
    if (!pllSetupDistributionWithUpdate(&setup->out_dividers))
        return false;
    if (!pllCalibrateAll())
        return false;
    if (!pllSyncAllDistDividers())
        return false;
    return true;
}

void ad9545_reset(void)
{
    // toggle reset_b pin
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
}

bool ad9545_gpio_test(void)
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
void ad9545_gpio_init(void)
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
