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

#include "dev_pll.h"

#include "stm32f7xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include "bsp.h"
#include "ad9545_i2c_hal.h"
#include "ad9545_setup.h"
#include "ad9545_status.h"
#include "dev_pll_print.h"
#include "dev_types.h"
#include "ansi_escape_codes.h"
#include "logbuffer.h"
#include "cmsis_os.h"
#include "app_shared_data.h"

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return HAL_GetTick() - stateStartTick;
}

typedef enum {
    DEV_OK       = HAL_OK,
    IIC_ERROR    = HAL_ERROR,
    IIC_BUSY     = HAL_BUSY,
    IIC_TIMEOUT  = HAL_TIMEOUT,
    DEV_ERROR    = 0x20U,
} OpStatusTypeDef;

static char *OpStatusErrorStr(OpStatusTypeDef status)
{
    switch(status) {
    case DEV_OK: return "Success";
    case IIC_ERROR: return "I2C error";
    case IIC_BUSY: return "I2C busy";
    case IIC_TIMEOUT: return "I2C timeout";
    case DEV_ERROR: return "Device error";
    default:return "";
    }
}

void DEBUG_PRINT_RET(const char *func, int ret)
{
    log_printf(LOG_ERR, "%s failed: %s, I2C error 0x%08lX\n",
           func, OpStatusErrorStr(ret), hPll->ErrorCode);
}

typedef union
{
  struct
  {
    uint32_t softreset0:1;
    uint32_t spi_config:6; // SPI only
    uint32_t softreset7:1;
  } b;
  uint8_t raw;
} REG_CONFIG_Type;

typedef union
{
  struct
  {
    uint32_t autosync_mode:2;
    uint32_t enable_ref_sync:1;
    uint32_t reserved:5;
  } b;
  uint8_t raw;
} Sync_Control_REG_Type;

enum {
    AD9545_REG1_CONFIG_0  = 0x0000,
    AD9545_REG1_CONFIG_1  = 0x0001,
    AD9545_REG_VENDOR_ID = 0x000C,
    AD9545_REG1_Sysclk_FB_DIV_Ratio = 0x0200,
    AD9545_REG1_Sysclk_Input = 0x0201,
    AD9545_REG5_Sysclk_Ref_Frequency = 0x0202,
    AD9545_REG3_Sysclk_Stability_Timer = 0x0207,
    AD9545_REG1_0280 = 0x0280,
    AD9545_REG1_0282 = 0x0282,
    AD9545_REG2_0285 = 0x0285,
    AD9545_REG5_0289 = 0x0289,

    AD9545_REG1_0300 = 0x0300,
    AD9545_REG1_0304 = 0x0304,

    AD9545_REG4_0400 = 0x0400,
    AD9545_REG8_0404 = 0x0404,
    AD9545_REG3_040C = 0x040C,
    AD9545_REG3_0410 = 0x0410,
    AD9545_REG2_0413 = 0x0413,

    AD9545_REG4_0440 = 0x0440,
    AD9545_REG8_0444 = 0x0444,
    AD9545_REG3_044C = 0x044C,
    AD9545_REG2_0450 = 0x0450,
    AD9545_REG2_0453 = 0x0453,

    AD9545_REG1_10D7 = 0x10D7,
    AD9545_REG1_10D8 = 0x10D8,
    AD9545_REG1_10D9 = 0x10D9,
    AD9545_REG1_10DA = 0x10DA,
    AD9545_REG1_10DB = 0x10DB,
    AD9545_REG1_10DC = 0x10DC,
    AD9545_REG1_1100 = 0x1100,
    AD9545_REG1_1112 = 0x1112,
    AD9545_REG1_1124 = 0x1124,
    AD9545_REG1_1200 = 0x1200,
    AD9545_REG1_1201 = 0x1201,
    AD9545_REG1_1202 = 0x1202,
    AD9545_REG1_1203 = 0x1203,
    AD9545_REG4_1204 = 0x1204,
    AD9545_REG4_1208 = 0x1208,
    AD9545_REG4_120C = 0x120C,
    AD9545_REG3_1210 = 0x1210,
    AD9545_REG3_1213 = 0x1213,
    AD9545_REG3_1217 = 0x1217,
    AD9545_REG1_14D7 = 0x14D7,
    AD9545_REG1_14D8 = 0x14D8,
    AD9545_REG1_14D9 = 0x14D9,
    AD9545_REG1_14DA = 0x14DA,
    AD9545_REG1_14DB = 0x14DB,
    AD9545_REG1_14DC = 0x14DC,
    AD9545_REG1_1500 = 0x1500,
    AD9545_REG1_1512 = 0x1512,
    AD9545_REG1_1600 = 0x1600,
    AD9545_REG1_1601 = 0x1601,
    AD9545_REG1_1602 = 0x1602,
    AD9545_REG1_1603 = 0x1603,
    AD9545_REG4_1604 = 0x1604,
    AD9545_REG4_1608 = 0x1608,
    AD9545_REG4_160C = 0x160C,
    AD9545_REG3_1610 = 0x1610,
    AD9545_REG3_1613 = 0x1613,
    AD9545_REG3_1617 = 0x1617,
    AD9545_REG1_2000 = 0x2000,
    AD9545_REG1_2001 = 0x2001,
    AD9545_REG1_200C = 0x200C,
    AD9545_REG1_2100 = 0x2100,
    AD9545_REG1_2102 = 0x2102,
    AD9545_REG1_2103 = 0x2103,
    AD9545_REG1_2104 = 0x2104,
    AD9545_REG1_2105 = 0x2105,
    AD9545_REG1_2106 = 0x2106,
    AD9545_REG1_2107 = 0x2107,
    AD9545_REG1_2200 = 0x2200,
    AD9545_REG1_2202 = 0x2202,
    AD9545_REG1_2203 = 0x2203,
    AD9545_REG1_2205 = 0x2205,
    AD9545_REG1_2207 = 0x2207,
    AD9545_LIVE_REG1_3000 = 0x3000,
    AD9545_LIVE_REG1_3001 = 0x3001,
    AD9545_REG1_3002 = 0x3002,
    AD9545_REG2_INT_THERM = 0x3003,
    AD9545_REG1_3005 = 0x3005,
    AD9545_REG1_3006 = 0x3006,
    AD9545_REG1_3007 = 0x3007,
    AD9545_REG1_3008 = 0x3008,
    AD9545_REG1_3009 = 0x3009,
    AD9545_REG1_300A = 0x300A,

    AD9545_REG1_3100 = 0x3100,
    AD9545_REG1_3101 = 0x3101,
    AD9545_REG1_3102 = 0x3102,
    AD9545_REG6_3103 = 0x3103,
    AD9545_REG2_3109 = 0x3109,
    AD9545_REG2_310B = 0x310B,
    AD9545_REG1_310D = 0x310D,
    AD9545_REG1_310E = 0x310E,

    AD9545_REG1_3200 = 0x3200,
    AD9545_REG1_3201 = 0x3201,
    AD9545_REG1_3202 = 0x3202,
    AD9545_REG6_3203 = 0x3203,
    AD9545_REG2_3209 = 0x3209,
    AD9545_REG2_320B = 0x320B,
    AD9545_REG1_320D = 0x320D,
    AD9545_REG1_320E = 0x320E,
};

enum {
    AD9545_VENDOR_ID = 0x0456
};

static const uint8_t AD9545_OPER_CONTROL_DEFAULT = 0x0A; // shutdown RefAA, RefBB

static OpStatusTypeDef pllIoUpdate(Dev_ad9545 *d)
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

static OpStatusTypeDef pllRegisterPulseBit_unused(Dev_ad9545 *d, uint16_t address, uint8_t bitmask)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    uint8_t data = 0;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_read1(address, &data)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(address, data | bitmask)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(address, data & ~bitmask)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

DeviceStatus pllDetect(Dev_ad9545 *d)
{
    int devicePresent = 0;
    int deviceError = 0;
    HAL_StatusTypeDef ret = ad9545_detect();
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
    d->present = (devicePresent && !deviceError) ? DEVICE_NORMAL : DEVICE_FAIL;
    return d->present;
}

static OpStatusTypeDef pllSoftwareReset(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    ret = ad9545_write1(0x0000, 0x81);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_write1(0x0000, 0);
    if (ret != HAL_OK)
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupSysclk(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    PllSysclkSetup_TypeDef sysclkSetup = {0};
    init_PllSysclkSetup(&sysclkSetup);

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_FB_DIV_Ratio, sysclkSetup.Sysclk_FB_DIV_Ratio)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_Input, sysclkSetup.Sysclk_Input)))
        goto err;
    if (HAL_OK != (ret = ad9545_write5(AD9545_REG5_Sysclk_Ref_Frequency, sysclkSetup.sysclk_Ref_Frequency_milliHz)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_Sysclk_Stability_Timer, sysclkSetup.Sysclk_Stability_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0280, sysclkSetup.TDC_Compensation_Source)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0282, sysclkSetup.DPLL_Compensation_Source)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0285, sysclkSetup.AuxDPLL_Bandwidth)))
        goto err;
    if (HAL_OK != (ret = ad9545_write5(AD9545_REG5_0289, sysclkSetup.CompensationValue)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(0x2903, sysclkSetup.Temperature_Low_Threshold)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(0x2905, sysclkSetup.Temperature_Hihg_Threshold)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllCalibrateApll_unused(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    // calibrate APLL 0 (requires IO Update, not autoclearing)
    uint8_t OpControlChannel0 = 0x02; // calibrate
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2100, OpControlChannel0)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    OpControlChannel0 = 0; // clear calibrate bit
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2100, OpControlChannel0)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    // calibrate APLL 1 (requires IO Update, not autoclearing)
    uint8_t OpControlChannel1 = 0x02; // calibrate
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2200, OpControlChannel1)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    OpControlChannel1 = 0; // clear calibrate bit
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2200, OpControlChannel1)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllResetOutputDividers_unused(Dev_ad9545 *d)
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
    if (HAL_OK != (ret = pllIoUpdate(d)))
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
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupOutputDrivers(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    Pll_OutputDrivers_Setup_TypeDef setup = {0};
    init_Pll_OutputDrivers_Setup(&setup);
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D7, setup.Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D8, setup.Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10D9, setup.Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14D7, setup.Driver_Config.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14D8, setup.Driver_Config.raw)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllClearAutomute_unused(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2107, 0x10)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2207, 0x10)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2107, 0x00)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2207, 0x00)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupDistributionWithUpdate(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    Pll_OutputDividers_Setup_TypeDef setup = {0};
    init_Pll_OutputDividers_Setup(&setup);
    // channel 0
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DA, setup.Secondary_Clock_Path_0)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DC, setup.Automute_Control_0)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1100, setup.Distribution_Divider_0_A)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1112, setup.Distribution_Divider_0_B)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1124, setup.Distribution_Divider_0_C)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate(d)))
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
    Sync_Control_0.b.enable_ref_sync = setup.enable_ref_sync_0;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DB, Sync_Control_0.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    Sync_Control_0.b.autosync_mode = setup.autosync_mode_0;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_10DB, Sync_Control_0.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    // channel 1
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DA, setup.Secondary_Clock_Path_1)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DC, setup.Automute_Control_1)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1500, setup.Distribution_Divider_1_A)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_1512, setup.Distribution_Divider_1_B)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    // When using reference synchronization in conjunction with
    // autosync mode (that is, autosync mode = 1, 2, or 3), be sure to
    // program enable DPLLx reference sync = 1 and assert the I/O_
    // UPDATE bit prior to programming autosync mode = 1, 2, or 3.
    Sync_Control_REG_Type Sync_Control_1;
    Sync_Control_1.raw = 0;
    Sync_Control_1.b.enable_ref_sync = setup.enable_ref_sync_1;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DB, Sync_Control_1.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    Sync_Control_1.b.autosync_mode = setup.autosync_mode_1;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_14DB, Sync_Control_1.raw)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupRef(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    PllRefSetup_TypeDef refSetup = {0};
    init_PllRefSetup(&refSetup);
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0300, refSetup.REFA_Receiver_Settings)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_0304, refSetup.REFB_Receiver_Settings)))
        goto err;

    // REFERENCE INPUT A (REFA) REGISTERS—REGISTER 0x0400 TO REGISTER 0x0414

    if (HAL_OK != (ret = ad9545_write4(AD9545_REG4_0400, refSetup.REFA_R_Divider)))
        goto err;
    if (HAL_OK != (ret = ad9545_write8(AD9545_REG8_0404, refSetup.REFA_Input_Period)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_040C, refSetup.REFA_Offset_Limit)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_0410, refSetup.REFA_Validation_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0413, refSetup.REFA_Jitter_Tolerance)))
        goto err;

    // REFERENCE INPUT B (REFB) REGISTERS—REGISTER 0x0440 TO REGISTER 0x0454

    if (HAL_OK != (ret = ad9545_write4(AD9545_REG4_0440, refSetup.REFB_R_Divider)))
        goto err;
    if (HAL_OK != (ret = ad9545_write8(AD9545_REG8_0444, refSetup.REFB_Input_Period)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_044C, refSetup.REFB_Offset_Limit)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0450, refSetup.REFB_Validation_Timer)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(AD9545_REG2_0453, refSetup.REFB_Jitter_Tolerance)))
        goto err;

    uint8_t OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, OpControlGlobal)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllWriteProfile(Dev_ad9545 *d, PllChannel_TypeDef channel, int profileIndex, Pll_DPLL_Profile_TypeDef profile)
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

static OpStatusTypeDef pllSetupDPLLChannel(Dev_ad9545 *d, PllChannel_TypeDef channel)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    Pll_DPLL_Setup_TypeDef dpll = {0};
    init_DPLL_Setup(&dpll, channel);
    uint16_t reg_offset = 0x0;
    switch(channel) {
    case DPLL0:
        break;
    case DPLL1:
        reg_offset = 0x400;
        break;
    }

    // DPLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write6(reg_offset + 0x1000, dpll.Freerun_Tuning_Word)))
        goto err;
    if (HAL_OK != (ret = ad9545_write3(reg_offset + 0x1006, dpll.FTW_Offset_Clamp)))
        goto err;

    // APLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x1081, dpll.APLL_M_Divider)))
        goto err;

    for (int i=0; i<2; i++)
        if (HAL_OK != (ret = pllWriteProfile(d, channel, i, dpll.profile[i])))
            goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupDPLL(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = pllSetupDPLLChannel(d, DPLL0)))
        goto err;
    if (HAL_OK != (ret = pllSetupDPLLChannel(d, DPLL1)))
        goto err;
    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSetupDPLLMode(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    Pll_DPLLMode_Setup_TypeDef dpll_mode = {0};
    init_Pll_DPLLMode_Setup(&dpll_mode);
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2105, dpll_mode.dpll0_mode.raw)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2205, dpll_mode.dpll1_mode.raw)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllCalibrateSysclk(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x04)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x04)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllCalibrateAll(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x02)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x02)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllSyncAllDistDividers(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT | 0x08)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, AD9545_OPER_CONTROL_DEFAULT & ~0x08)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllReadRefStatus(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = DEV_ERROR;
    uint8_t refa;
    ret = ad9545_read1(AD9545_REG1_3005, &refa);
    if (ret != HAL_OK)
        goto err;
    d->status.ref[0].raw = refa;

    uint8_t refb;
    ret = ad9545_read1(AD9545_REG1_3005, &refb);
    if (ret != HAL_OK)
        goto err;
    d->status.ref[2].raw = refb;

    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllReadDPLLChannelStatus(Dev_ad9545 *d, PllChannel_TypeDef channel)
{
    HAL_StatusTypeDef ret = DEV_ERROR;

    uint16_t reg_offset = 0x0;
    switch(channel) {
    case DPLL0:
        break;
    case DPLL1:
        reg_offset = 0x100;
        break;
    }
    DPLL_Status *dpll_status = &d->status.dpll[channel];

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

static OpStatusTypeDef pllReadStatus(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = DEV_ERROR;

    ret = pllIoUpdate(d);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_LIVE_REG1_3000, &d->status.eeprom.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_LIVE_REG1_3001, &d->status.sysclk.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read1(AD9545_REG1_3002, &d->status.misc.raw);
    if (ret != HAL_OK)
        goto err;
    ret = ad9545_read2(AD9545_REG2_INT_THERM, (uint16_t *)&d->status.internal_temp);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadRefStatus(d);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadDPLLChannelStatus(d, DPLL0);
    if (ret != HAL_OK)
        goto err;

    ret = pllReadDPLLChannelStatus(d, DPLL1);
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
    return DEV_OK;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllReadSysclkStatus(Dev_ad9545 *d)
{
    // read sysclk status
    HAL_StatusTypeDef ret = ad9545_read1(AD9545_LIVE_REG1_3001, &d->status.sysclk.raw);
    if (ret != HAL_OK)
        goto err;
    return ret;
err:
    DEBUG_PRINT_RET(__func__, ret);
    return ret;
}

static OpStatusTypeDef pllReadAllRegisters(Dev_ad9545 *d)
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

static void reset_I2C_Pll(void)
{
    __HAL_I2C_DISABLE(hPll);
    __HAL_I2C_ENABLE(hPll);
}

/*
1. Configure the system clock.
2. Configure the DPLL (digital PLL)
3. Configure the reference inputs.
4. Configure the output drivers
5. Configure the status pins (optional)
*/

void pllRun(Dev_ad9545 *d)
{
    const PllState oldState = d->fsm_state;
    switch(d->fsm_state) {
    case PLL_STATE_INIT:
        if (enable_pll_run && enable_power)
            d->fsm_state = PLL_STATE_RESET;
        break;
    case PLL_STATE_RESET:
        reset_I2C_Pll();
        pllReset();
        osDelay(50);
        pllDetect(d);
        if (DEVICE_NORMAL == d->present) {
            if (DEV_OK != pllSoftwareReset(d)) {
                d->fsm_state = PLL_STATE_ERROR;
                break;
            }
            d->fsm_state = PLL_STATE_SETUP_SYSCLK;
            break;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "PLL AD9545 not found");
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        break;
    case PLL_STATE_SETUP_SYSCLK:
        if (DEV_OK != pllSetupSysclk(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllCalibrateSysclk(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->fsm_state = PLL_STATE_SYSCLK_WAITLOCK;
        break;
    case PLL_STATE_SYSCLK_WAITLOCK:
        if (d->status.sysclk.b.locked && d->status.sysclk.b.stable) {
            d->fsm_state = PLL_STATE_SETUP;
        }
        if (stateTicks() > 2000) {
            log_put(LOG_ERR, "PLL sysclock lock timeout");
            d->fsm_state = PLL_STATE_ERROR;
        }
        break;
    case PLL_STATE_SETUP:
        if (DEV_OK != pllSetupOutputDrivers(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupDPLL(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupDPLLMode(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupRef(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupDistributionWithUpdate(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllCalibrateAll(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSyncAllDistDividers(d)) {
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->fsm_state = PLL_STATE_RUN;
        break;
    case PLL_STATE_RUN:
        if (!d->status.sysclk.b.locked) {
            log_put(LOG_ERR, "PLL sysclock unlocked");
            d->fsm_state = PLL_STATE_ERROR;
            break;
        }
        d->recoveryCount = 0;
        break;
    case PLL_STATE_ERROR:
        if (d->recoveryCount > 3) {
            d->fsm_state = PLL_STATE_FATAL;
            log_put(LOG_CRIT, "PLL fatal error");
            break;
        }
        if (stateTicks() > 1000) {
            d->recoveryCount++;
            d->fsm_state = PLL_STATE_INIT;
        }
        break;
    case PLL_STATE_FATAL:
        d->present = DEVICE_FAIL;
        if (stateTicks() > 2000) {
            // recover
            d->recoveryCount = 0;
            d->fsm_state = PLL_STATE_INIT;
        }
        break;
    default:
        d->fsm_state = PLL_STATE_INIT;
    }

    if (d->fsm_state != PLL_STATE_INIT &&
            d->fsm_state != PLL_STATE_RESET &&
            d->fsm_state != PLL_STATE_ERROR &&
            d->fsm_state != PLL_STATE_FATAL) {
        if (DEV_OK != pllReadSysclkStatus(d)) {
            d->fsm_state = PLL_STATE_ERROR;
        }
    }
    if (d->fsm_state == PLL_STATE_RUN) {
        if (DEV_OK != pllReadStatus(d)) {
            d->fsm_state = PLL_STATE_ERROR;
        }
    }
    int stateChanged = oldState != d->fsm_state;
    if (stateChanged) {
        stateStartTick = HAL_GetTick();
    }
    if (stateChanged && (oldState != PLL_STATE_RESET)) {
        if (d->fsm_state == PLL_STATE_ERROR) {
            log_put(LOG_ERR, "PLL interface error");
        }
        if (d->fsm_state == PLL_STATE_RUN) {
            log_put(LOG_INFO, "PLL started");
        }
    }
}
