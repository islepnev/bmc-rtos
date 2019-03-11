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
#include "ad9545_i2c_hal.h"
#include "ad9545_setup.h"
#include "dev_types.h"
#include "ansi_escape_codes.h"
#include "cmsis_os.h"

#define DEBUG_PRINT_RET(x)     printf("%s failed: %s, I2C error 0x%08lX\n", __func__, OpStatusErrorStr(x), hPll->ErrorCode)

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

    AD9545_REG1_0300 = 0x0300,
    AD9545_REG1_0304 = 0x0304,

    AD9545_REG4_0400 = 0x0400,
    AD9545_REG8_0404 = 0x0404,

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
    AD9545_REG1_3000 = 0x3000,
    AD9545_REG1_3001 = 0x3001,
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
    AD9545_REG1_3200 = 0x3200,
    AD9545_REG1_3201 = 0x3201,
    AD9545_REG1_3202 = 0x3202,
    AD9545_REG2_3209 = 0x3209,
    AD9545_REG2_320B = 0x320B,
};

enum {
    AD9545_VENDOR_ID = 0x0456
};

static const uint8_t AD9545_OPER_CONTROL_DEFAULT = 0x0A; // shutdown RefAA, RefBB

void pllReset(Dev_ad9545 *d)
{
    // toggle reset_b pin
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
//    osDelay(10);
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
    osDelay(50);
    // wait 50 ms (see FSM)
}

static OpStatusTypeDef pllIoUpdate(Dev_ad9545 *d)
{
    uint8_t data = 1;
    HAL_StatusTypeDef ret = ad9545_write1(0x000F, data);
    if (ret != HAL_OK)
        goto err;
    osDelay(1);
    return ret;
err:
//    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllRegisterPulseBit(Dev_ad9545 *d, uint16_t address, uint8_t bitmask)
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
    if (HAL_OK != (ret = ad9545_write1(address, data)))
        goto err;
    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

DeviceStatus pllDetect(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = ad9545_detect();
    d->present = (HAL_OK == ret);
    if (d->present) {
        uint32_t data = 0;
        ad9545_read3(AD9545_REG_VENDOR_ID, &data);
        d->present = (data == AD9545_VENDOR_ID);
    }
    /*
//    pllSendByte(AD9545_REG_VENDOR_ID);
//    pllReceiveByte(&data);
    pllReadRegister(AD9545_REG_VENDOR_ID, &data);
    d->present = (data == AD9545_VENDOR_ID);
//    pllReadRegister(AD9545_REG_INT_THERM, &data);
//    printf("PLL therm: %04lX\n", data);
*/
    // scratchpad test
    uint32_t test = 0x87654321;
    ad9545_write4(0x0020, test);
    uint32_t data = 0;
    ad9545_read4(0x0020, &data);
    d->present &= (data == test);
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

    if (0) for (int n=0; ; n++) {
        // scratchpad test
        uint32_t test = 0x87654321;
        ret = ad9545_write4(0x0020, test);
        if (ret != HAL_OK)
            goto err;
        uint32_t data = 0;
        ret = ad9545_read4(0x0020, &data);
        if (ret != HAL_OK)
            goto err;
        if (data != test) {
            printf("%08lx %08lx\n", data, test);
        }
        if (n%1000 == 0) {
            printf(".");
            fflush(stdout);
        }
    }

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllSetupSysclk(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    PllSysclkSetup_TypeDef sysclkSetup;
    init_PllSysclkSetup(&sysclkSetup);

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_FB_DIV_Ratio, sysclkSetup.Sysclk_FB_DIV_Ratio)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_Sysclk_Input, sysclkSetup.Sysclk_Input)))
        goto err;

    if (HAL_OK != (ret = ad9545_write5(AD9545_REG5_Sysclk_Ref_Frequency, sysclkSetup.sysclk_Ref_Frequency_milliHz)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(AD9545_REG3_Sysclk_Stability_Timer, sysclkSetup.Sysclk_Stability_Timer)))
        goto err;

    if (HAL_OK != (ret = ad9545_write2(0x2903, sysclkSetup.Temperature_Low_Threshold)))
        goto err;
    if (HAL_OK != (ret = ad9545_write2(0x2905, sysclkSetup.Temperature_Hihg_Threshold)))
        goto err;

    if (HAL_OK != (ret = pllIoUpdate(d)))
        goto err;

    // calibrate sysclk (requires IO Update, not autoclearing)
    if (HAL_OK != (ret = pllRegisterPulseBit(d, AD9545_REG1_2000, 0x04)))
        goto err;

//    uint32_t OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT | 0x04; // calibrate
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, OpControlGlobal)))
//        goto err;
//    if (HAL_OK != (ret = pllIoUpdate(d)))
//        goto err;
//    OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT; // clear calibrate bit
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, OpControlGlobal)))
//        goto err;
//    // I/O registers update
//    if (HAL_OK != (ret = pllIoUpdate(d)))
//        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllCalibrateApll(Dev_ad9545 *d)
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllResetOutputDividers(Dev_ad9545 *d)
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllSetupOutputDrivers(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    Pll_OutputDrivers_Setup_TypeDef setup;
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllClearAutomute(Dev_ad9545 *d)
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllSetupDistribution(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    Pll_OutputDividers_Setup_TypeDef setup;
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllSetupRef(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    PllRefSetup_TypeDef refSetup;
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

    // REFERENCE INPUT B (REFB) REGISTERS—REGISTER 0x0440 TO REGISTER 0x0454

    if (HAL_OK != (ret = ad9545_write4(0x0440, refSetup.REFB_R_Divider)))
        goto err;

    if (HAL_OK != (ret = ad9545_write8(0x0444, refSetup.REFB_Input_Period)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

typedef enum {
    DPLL0 = 0,
    DPLL1 = 1,
} PllChannel_TypeDef;

static OpStatusTypeDef pllSetupDPLL(Dev_ad9545 *d, PllChannel_TypeDef channel)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    Pll_DPLL_Setup_TypeDef dpll;
    uint16_t reg_offset = 0x0;
    switch(channel) {
    case DPLL0:
        init_DPLL0_Setup(&dpll);
        break;
    case DPLL1:
        init_DPLL1_Setup(&dpll);
        reg_offset = 0x400;
        break;
    }

    // DPLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write6(reg_offset + 0x1000, dpll.Freerun_Tuning_Word)))
        goto err;

    // APLL CHANNEL REGISTERS
    if (HAL_OK != (ret = ad9545_write1(reg_offset + 0x1081, dpll.APLL_M_Divider)))
        goto err;

    // DPLL TRANSLATION PROFILE 0.0 REGISTERS
    if (HAL_OK != (ret = ad9545_write1(reg_offset + AD9545_REG1_1200, dpll.Priority_and_Enable)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + AD9545_REG1_1201, dpll.Profile_Ref_Source)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + AD9545_REG1_1202, dpll.ZD_Feedback_Path)))
        goto err;

    if (HAL_OK != (ret = ad9545_write1(reg_offset + AD9545_REG1_1203, dpll.Feedback_Mode)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + AD9545_REG4_1204, dpll.Loop_BW)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + AD9545_REG4_1208, dpll.Hitless_FB_Divider)))
        goto err;

    if (HAL_OK != (ret = ad9545_write4(reg_offset + AD9545_REG4_120C, dpll.Buildout_FB_Divider)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + AD9545_REG3_1210, dpll.Buildout_FB_Fraction)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + AD9545_REG3_1213, dpll.Buildout_FB_Modulus)))
        goto err;

    if (HAL_OK != (ret = ad9545_write3(reg_offset + AD9545_REG3_1217, dpll.FastLock)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}
static OpStatusTypeDef pllSetupDPLL0(Dev_ad9545 *d)
{
    Pll_DPLL_Setup_TypeDef dpll;
    init_DPLL0_Setup(&dpll);
    return pllSetupDPLL(d, DPLL0);
}
static OpStatusTypeDef pllSetupDPLL1(Dev_ad9545 *d)
{
    Pll_DPLL_Setup_TypeDef dpll;
    init_DPLL1_Setup(&dpll);
    return pllSetupDPLL(d, DPLL1);
}

static OpStatusTypeDef pllSetupOperationalControl(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

//    uint8_t Divider_Q0A = 0;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2102, Divider_Q0A)))
//        goto err;
//    uint8_t Divider_Q0B = 0;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2103, Divider_Q0B)))
//        goto err;
//    uint8_t Divider_Q0C = 0;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2104, Divider_Q0C)))
//        goto err;
    Pll_DPLLMode_Setup_TypeDef dpll_mode;
    init_Pll_DPLLMode_Setup(&dpll_mode);
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2105, dpll_mode.dpll0_mode.raw)))
        goto err;
//    uint8_t Divider_Q1A = 0;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2202, Divider_Q1A)))
//        goto err;
//    uint8_t Divider_Q1B = 0;
//    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2203, Divider_Q1B)))
//        goto err;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2205, dpll_mode.dpll1_mode.raw)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllManualOutputSync(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if (HAL_OK != (ret = pllRegisterPulseBit(d, AD9545_REG1_2000, 0x08)))
        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllSetup(Dev_ad9545 *d)
{
//    int reset_b = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin));
//    printf("reset_b = %d\n", reset_b);
    HAL_StatusTypeDef ret = HAL_ERROR;
    // config1
    if (0) {
        uint8_t config1 = 0; // 1 << 5;
        ret = ad9545_write1(0x0001, config1);
        if (ret != HAL_OK)
            return ret;
        uint8_t data = 0;
        ret = ad9545_read1(0x0001, &data);
        if (ret != HAL_OK)
            return ret;
        printf("config1 %02X\n", data);
        if (data != config1) {
            printf("config1 write error\n");
            return HAL_ERROR;
        }
    }
    uint8_t OpControlGlobal = AD9545_OPER_CONTROL_DEFAULT;
    if (HAL_OK != (ret = ad9545_write1(AD9545_REG1_2000, OpControlGlobal)))
        goto err;

//    // I/O registers update
//    if (HAL_OK != (ret = pllIoUpdate(d)))
//        goto err;

    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllReadRefStatus(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = DEV_ERROR;
    // read RefA status
    uint8_t refa;
    ret = ad9545_read1(AD9545_REG1_3005, &refa);
    if (ret != HAL_OK)
        goto err;
    printf("Ref A  %02X %s%s%s%s%s%s\n",
           refa,
           (refa & 0x01) ? " SLOW" : "",
           (refa & 0x02) ? " FAST" : "",
           (refa & 0x04) ? " JITTER" : "",
           (refa & 0x08) ? " FAULT" : "",
           (refa & 0x10) ? " VALID" : "",
           (refa & 0x20) ? " LOS" : ""
                          );

//    // read RefAA status
//    uint8_t refaa;
//    ret = ad9545_read1(AD9545_REG1_3006, &refaa);
//    if (ret != HAL_OK)
//        goto err;
//    printf("Ref AA %02X %s%s%s%s%s%s\n",
//           refaa,
//           (refaa & 0x01) ? " SLOW" : "",
//           (refaa & 0x02) ? " FAST" : "",
//           (refaa & 0x04) ? " JITTER" : "",
//           (refaa & 0x08) ? " FAULT" : "",
//           (refaa & 0x10) ? " VALID" : "",
//           (refaa & 0x20) ? " LOS" : ""
//                          );

    // read RefB status
    uint8_t refb;
    ret = ad9545_read1(AD9545_REG1_3007, &refb);
    if (ret != HAL_OK)
        goto err;
    printf("Ref B  %02X %s%s%s%s%s%s\n",
           refb,
           (refb & 0x01) ? " SLOW" : "",
           (refb & 0x02) ? " FAST" : "",
           (refb & 0x04) ? " JITTER" : "",
           (refb & 0x08) ? " FAULT" : "",
           (refb & 0x10) ? " VALID" : "",
           (refb & 0x20) ? " LOS" : ""
                          );

//    // read RefBB status
//    uint8_t refbb;
//    ret = ad9545_read1(AD9545_REG1_3008, &refbb);
//    if (ret != HAL_OK)
//        goto err;
//    printf("Ref BB %02X %s%s%s%s%s%s\n",
//           refbb,
//           (refbb & 0x01) ? " SLOW" : "",
//           (refbb & 0x02) ? " FAST" : "",
//           (refbb & 0x04) ? " JITTER" : "",
//           (refbb & 0x08) ? " FAULT" : "",
//           (refbb & 0x10) ? " VALID" : "",
//           (refbb & 0x20) ? " LOS" : ""
//                          );
    return ret;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllReadStatus(Dev_ad9545 *d)
{
    HAL_StatusTypeDef ret = DEV_ERROR;
    // read eeprom status
    uint8_t eepromStatus;
    ret = ad9545_read1(AD9545_REG1_3000, &eepromStatus);
    if (ret != HAL_OK)
        goto err;
//    printf("eeprom status %02X\n", eepromStatus);

    // read misc status
    uint8_t miscStatus;
    ret = ad9545_read1(AD9545_REG1_3002, &miscStatus);
    if (ret != HAL_OK)
        goto err;
    printf("misc status %02X %s%s%s\n",
           miscStatus,
           (miscStatus & 0x01) ? " TEMP" : "",
           (miscStatus & 0x02) ? " AUX_DPLL_LOCK" : "",
           (miscStatus & 0x04) ? " AUX_DPLL_REF_FAULT" : ""
                                 );

    // read temp
    uint16_t temp;
    ret = ad9545_read2(AD9545_REG2_INT_THERM, &temp);
    if (ret != HAL_OK)
        goto err;
    printf("internal temp %d C\n", temp/128);

    ret = pllReadRefStatus(d);
    if (ret != HAL_OK)
        goto err;

    // read DPLL0 translation profile
    uint8_t dpll0TrProfile;
    ret = ad9545_read1(AD9545_REG1_3009, &dpll0TrProfile);
    if (ret != HAL_OK)
        goto err;
    printf("DPLL0 translation profile %s%s%s%s%s%s%s\n",
           (dpll0TrProfile==0) ? "none" : "",
           (dpll0TrProfile==0x1) ? "0.0" : "",
           (dpll0TrProfile==0x2) ? "0.1" : "",
           (dpll0TrProfile==0x4) ? "0.2" : "",
           (dpll0TrProfile==0x8) ? "0.3" : "",
           (dpll0TrProfile==0x10) ? "0.4" : "",
           (dpll0TrProfile==0x20) ? "0.5" : ""
           );

    // read DPLL1 translation profile
    uint8_t dpll1TrProfile;
    ret = ad9545_read1(AD9545_REG1_300A, &dpll1TrProfile);
    if (ret != HAL_OK)
        goto err;
    printf("DPLL1 translation profile %s%s%s%s%s%s%s\n",
           (dpll1TrProfile==0) ? "none" : "",
           (dpll1TrProfile==0x1) ? "1.0" : "",
           (dpll1TrProfile==0x2) ? "1.1" : "",
           (dpll1TrProfile==0x4) ? "1.2" : "",
           (dpll1TrProfile==0x8) ? "1.3" : "",
           (dpll1TrProfile==0x10) ? "1.4" : "",
           (dpll1TrProfile==0x20) ? "1.5" : ""
           );


    // read PLL0 status
    uint8_t dpll0LockStatus;
    ret = ad9545_read1(AD9545_REG1_3100, &dpll0LockStatus);
    if (ret != HAL_OK)
        goto err;
    printf("PLL0 lock status %02X %s%s%s%s%s%s\n",
           dpll0LockStatus,
           (dpll0LockStatus & 0x01) ? " ALL_LOCK" : "",
           (dpll0LockStatus & 0x02) ? " D_PHASE_LOCK" : "",
           (dpll0LockStatus & 0x04) ? " D_FREQ_LOCK" : "",
           (dpll0LockStatus & 0x08) ? " A_LOCK" : "",
           (dpll0LockStatus & 0x10) ? " A_CAL_BUSY" : "",
           (dpll0LockStatus & 0x20) ? " A_CALIBRATED" : ""
                                 );

    // read PLL0 status
    uint8_t dpll0OperStatus;
    ret = ad9545_read1(AD9545_REG1_3101, &dpll0OperStatus);
    if (ret != HAL_OK)
        goto err;
    printf("PLL0 oper status %02X %s%s%s%s profile %d\n",
           dpll0OperStatus,
           (dpll0OperStatus & 0x01) ? " FREERUN" : "",
           (dpll0OperStatus & 0x02) ? " HOLDOVER" : "",
           (dpll0OperStatus & 0x04) ? " REF_SWITCH" : "",
           (dpll0OperStatus & 0x08) ? " ACTIVE" : "",
           (dpll0OperStatus >> 4) & 0x3
                                 );
    // read PLL0 state
    uint8_t dpll0OperState;
    ret = ad9545_read1(AD9545_REG1_3102, &dpll0OperState);
    if (ret != HAL_OK)
        goto err;
    printf("PLL0 state %02X %s%s%s%s%s\n",
           dpll0OperState,
           (dpll0OperState & 0x01) ? " HIST" : "",
           (dpll0OperState & 0x02) ? " FREQ_CLAMP" : "",
           (dpll0OperState & 0x04) ? " PHASE_SLEW_LIMIT" : "",
           (dpll0OperState & 0x10) ? " FACQ_ACT" : "",
           (dpll0OperState & 0x20) ? " FACQ_DONE" : ""
           );

//    // read PLL0 FTW History
//    uint64_t dpll0FtwHistory;
//    ret = ad9545_read6(AD9545_REG6_3103, &dpll0FtwHistory);
//    if (ret != HAL_OK)
//        return ret;
//    printf("PLL0 FTW history %llu\n", dpll0FtwHistory);

    // read PLL0 PLD/FLD tub
    uint16_t dpll0PldTub;
    ret = ad9545_read2(AD9545_REG2_3109, &dpll0PldTub);
    if (ret != HAL_OK)
        goto err;
    uint16_t dpll0FldTub;
    ret = ad9545_read2(AD9545_REG2_310B, &dpll0FldTub);
    if (ret != HAL_OK)
        goto err;
    printf("PLL0 PLD tub %d, FLD tub %d\n", dpll0PldTub, dpll0FldTub);


    // read PLL1 status
    {
    uint8_t dpll1LockStatus;
    ret = ad9545_read1(AD9545_REG1_3200, &dpll1LockStatus);
    if (ret != HAL_OK)
        return ret;
    printf("PLL1 lock status %02X %s%s%s%s%s%s\n",
           dpll1LockStatus,
           (dpll1LockStatus & 0x01) ? " ALL_LOCK" : "",
           (dpll1LockStatus & 0x02) ? " D_PHASE_LOCK" : "",
           (dpll1LockStatus & 0x04) ? " D_FREQ_LOCK" : "",
           (dpll1LockStatus & 0x08) ? " A_LOCK" : "",
           (dpll1LockStatus & 0x10) ? " A_CAL_BUSY" : "",
           (dpll1LockStatus & 0x20) ? " A_CALIBRATED" : ""
                                 );

    // read PLL1 status
    uint8_t dpll1OperStatus;
    ret = ad9545_read1(AD9545_REG1_3201, &dpll1OperStatus);
    if (ret != HAL_OK)
        return ret;
    printf("PLL1 oper status %02X %s%s%s%s profile %d\n",
           dpll1OperStatus,
           (dpll1OperStatus & 0x01) ? " FREERUN" : "",
           (dpll1OperStatus & 0x02) ? " HOLDOVER" : "",
           (dpll1OperStatus & 0x04) ? " REF_SWITCH" : "",
           (dpll1OperStatus & 0x08) ? " ACTIVE" : "",
           (dpll1OperStatus >> 4) & 0x3
                                 );
    // read PLL1 state
    uint8_t dpll1OperState;
    ret = ad9545_read1(AD9545_REG1_3202, &dpll1OperState);
    if (ret != HAL_OK)
        return ret;
    printf("PLL1 state %02X %s%s%s%s%s\n",
           dpll1OperState,
           (dpll1OperState & 0x01) ? " HIST" : "",
           (dpll1OperState & 0x02) ? " FREQ_CLAMP" : "",
           (dpll1OperState & 0x04) ? " PHASE_SLEW_LIMIT" : "",
           (dpll1OperState & 0x10) ? " FACQ_ACT" : "",
           (dpll1OperState & 0x20) ? " FACQ_DONE" : ""
           );
    }
    /*
    // read PLL1 status
    uint8_t dpll1LockStatus;
    ret = ad9545_read1(AD9545_REG1_3200, &dpll1LockStatus);
    if (ret != HAL_OK)
        return ret;
    printf("PLL1 status %02X %s%s%s%s%s%s\n",
           dpll1LockStatus,
           (dpll1LockStatus & 0x01) ? " ALL_LOCK" : "",
           (dpll1LockStatus & 0x02) ? " D_PHASE_LOCK" : "",
           (dpll1LockStatus & 0x04) ? " D_FREQ_LOCK" : "",
           (dpll1LockStatus & 0x08) ? " A_LOCK" : "",
           (dpll1LockStatus & 0x10) ? " A_CAL_BUSY" : "",
           (dpll1LockStatus & 0x20) ? " A_CALIBRATED" : ""
                                 );
*/
    // read PLL1 PLD/FLD tub
    uint16_t dpll1PldTub;
    ret = ad9545_read2(AD9545_REG2_3209, &dpll1PldTub);
    if (ret != HAL_OK)
        goto err;
    uint16_t dpll1FldTub;
    ret = ad9545_read2(AD9545_REG2_320B, &dpll1FldTub);
    if (ret != HAL_OK)
        goto err;
    printf("PLL1 PLD tub %d, FLD tub %d\n", dpll1PldTub, dpll1FldTub);

    // read
    for (int i=0x3000; i<=0x300A; i++) {
        uint8_t data;
        ret = ad9545_read1(i, &data);
        if (ret != HAL_OK)
            goto err;
        printf("[%04X] = %02X\n", i, data);
    }

    //    osDelay(1000);
    return DEV_OK;
err:
    DEBUG_PRINT_RET(ret);
    return ret;
}

static OpStatusTypeDef pllReadSysclkLocked(Dev_ad9545 *d)
{
    // read sysclk status
    uint8_t sysclkStatus;
    HAL_StatusTypeDef ret = ad9545_read1(AD9545_REG1_3001, &sysclkStatus);
    if (ret != HAL_OK)
        goto err;
    d->sysclkStatus.locked = sysclkStatus & 0x1;
    d->sysclkStatus.stable = sysclkStatus & 0x2;
    printf("sysclk: locked %d, stable %d\n", d->sysclkStatus.locked, d->sysclkStatus.stable);
    return ret;
err:
    d->sysclkStatus.locked = 0;
    d->sysclkStatus.stable = 0;
    DEBUG_PRINT_RET(ret);
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
    DEBUG_PRINT_RET(ret);
    return ret;
}

PllState pllState = PLL_STATE_INIT;

static char *pllStateStr(PllState pllState)
{
    switch(pllState) {
    case PLL_STATE_INIT: return "INIT";
    case PLL_STATE_RESET: return "RESET";
    case PLL_STATE_SETUP_SYSCLK: return "SETUP_SYSCLK";
    case PLL_STATE_SYSCLK_WAITLOCK: return "SYSCLK_WAITLOCK";
    case PLL_STATE_APLL_WAITCAL: return "APLL_WAITCAL";
    case PLL_STATE_SYSCLK_LOCKED: return "SYSCLK_LOCKED";
    case PLL_STATE_RUN: return "RUN";
    case PLL_STATE_ERROR: return "ERROR";
    default: return "unknown";
    }
}

static void reset_I2C_Pll(void)
{
    __HAL_I2C_DISABLE(hPll);
    __HAL_I2C_ENABLE(hPll);
}

void pllRun(Dev_ad9545 *d)
{
    const PllState oldState = pllState;
    switch(pllState) {
    case PLL_STATE_INIT:
        reset_I2C_Pll();
//        printf(ANSI_CLEARTERM ANSI_GOHOME);
//        pllReset(d);
        if (DEV_OK != pllSoftwareReset(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        pllState = PLL_STATE_RESET;
        break;
    case PLL_STATE_RESET:
        if (stateTicks() > 50) {
            pllState = PLL_STATE_SETUP_SYSCLK;
            break;
        }
        break;
    case PLL_STATE_SETUP_SYSCLK:
        if (DEV_OK != pllSetupSysclk(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        pllState = PLL_STATE_SYSCLK_WAITLOCK;
        break;
    case PLL_STATE_SYSCLK_WAITLOCK:
        if (DEV_OK != pllReadSysclkLocked(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (d->sysclkStatus.locked && d->sysclkStatus.stable) {
            pllCalibrateApll(d);
            pllState = PLL_STATE_APLL_WAITCAL;
        }
        if (stateTicks() > 2000) {
            printf("SYSCLK_WAITLOCK timeout\n");
            pllState = PLL_STATE_ERROR;
        }
        break;
    case PLL_STATE_APLL_WAITCAL:
        if (DEV_OK != pllReadSysclkLocked(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (d->sysclkStatus.locked && d->sysclkStatus.stable)
            pllState = PLL_STATE_SYSCLK_LOCKED;
        if (stateTicks() > 2000) {
            printf("SYSCLK_WAITLOCK timeout\n");
            pllState = PLL_STATE_ERROR;
        }
        break;
    case PLL_STATE_SYSCLK_LOCKED:
        if (DEV_OK != pllReadSysclkLocked(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (!d->sysclkStatus.locked) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupOutputDrivers(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllResetOutputDividers(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetup(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupRef(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupOperationalControl(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupDPLL0(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllSetupDPLL1(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        pllState = PLL_STATE_DIST_SYNC;
        break;
    case PLL_STATE_DIST_SYNC:
        if (stateTicks() > 1000) {
            if (DEV_OK != pllSetupDistribution(d)) {
                pllState = PLL_STATE_ERROR;
                break;
            }
//            if (DEV_OK != pllManualOutputSync(d)) {
//                pllState = PLL_STATE_ERROR;
//                break;
//            }
            pllState = PLL_STATE_RUN;
        }
        break;
    case PLL_STATE_RUN:
//        if (DEV_OK != pllClearAutomute(d)) {
//            pllState = PLL_STATE_ERROR;
//            break;
//        }
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (DEV_OK != pllReadSysclkLocked(d)) {
            pllState = PLL_STATE_ERROR;
            break;
        }
        if (!d->sysclkStatus.locked) {
            pllState = PLL_STATE_ERROR;
            break;
        }
//        if (DEV_OK != pllManualOutputSync(d)) {
//            pllState = PLL_STATE_ERROR;
//            break;
//        }
//        if (DEV_OK != pllReadStatus(d)) {
//            pllState = PLL_STATE_ERROR;
//            break;
//        }
//        if (DEV_OK != pllReadAllRegisters(d)) {
//            pllState = PLL_STATE_ERROR;
//            break;
//        }
        break;
    case PLL_STATE_ERROR:
        osDelay(100);
        if (stateTicks() > 1000) {
            pllState = PLL_STATE_INIT;
        }
        break;
    }
    d->pllState = pllState;

    //    if (pllState == PLL_STATE_SETUP_SYSCLK || pllState == PLL_STATE_SYSCLK_LOCKED) {
    //        if (pllReadSysclkLocked(d) != HAL_OK) {
    //            pllState = PLL_STATE_ERROR;
    //            break;
    //        }
    //    }

    if (pllState == PLL_STATE_DIST_SYNC || pllState == PLL_STATE_RUN) {
        printf(ANSI_CLEARTERM ANSI_GOHOME);
        if (DEV_OK != pllIoUpdate(d)) {
            pllState = PLL_STATE_ERROR;
        }
        if (DEV_OK != pllReadStatus(d)) {
            pllState = PLL_STATE_ERROR;
        }
        osDelay(50);
    }
//    osDelay(1000);
    if (oldState != pllState) {
        printf("PLL state changed %s -> %s\n", pllStateStr(oldState), pllStateStr(pllState));
        stateStartTick = HAL_GetTick();
    }
//    printf("PLL FSM state %s\n", pllStateStr(pllState));
}
