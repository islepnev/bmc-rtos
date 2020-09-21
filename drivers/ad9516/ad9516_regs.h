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

#ifndef AD9516_REGS_H
#define AD9516_REGS_H

#include <stdint.h>

typedef union {
    struct {
        uint32_t sdo_active:1;
        uint32_t lsb_first:1;
        uint32_t softreset:1;
        uint32_t long_instr:1;
        uint32_t long_instr_2:1; // mirrored bits
        uint32_t softreset_2:1;
        uint32_t lsb_first_2:1;
        uint32_t sdo_active_2:1;
    } b;
    uint8_t raw;
} AD9516_Serial_Config_REG_Type;

typedef union {
    struct {
        uint32_t pll_powerdown: 2;
        uint32_t chargepump_mode: 2;
        uint32_t chargepump_current: 3;
        uint32_t pfd_polarity: 1;
    } b;
    uint8_t raw;
} AD9516_PFD_CP_REG_Type;

typedef union {
    struct {
        uint8_t divider:3;
        uint8_t reserved:5;
    } b;
    uint8_t raw;
} AD9516_VCO_Divider_REG_Type;

typedef union {
    struct {
        uint8_t bypass_vco_div:1;
        uint8_t select_vco:1;
        uint8_t powerdown_vco_clk:1;
        uint8_t powerdown_vco_interface:1;
        uint8_t powerdown_clock_inp:1;
        uint8_t reserved:3;
    } b;
    uint8_t raw;
} AD9516_Input_Clocks_REG_Type;

typedef union {
    struct {
        uint8_t prescaler_p: 3;
        uint8_t bcounter_bypass: 1;
        uint8_t reset_all_counters: 1;
        uint8_t reset_a_b_counters: 1;
        uint8_t reset_r_counter: 1;
        uint8_t set_cp_pin: 1;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_1_REG_Type;

typedef union {
    struct {
        uint8_t antibacklash_pulse_width: 2;
        uint8_t status_pin_control: 6;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_2_REG_Type;

typedef union {
    struct {
        uint8_t vco_cal_now: 1;
        uint8_t vco_cal_divider: 2;
        uint8_t disable_dld: 1;
        uint8_t dld_window: 1;
        uint8_t ld_counter: 2;
        uint8_t reserved: 1;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_3_REG_Type;

typedef union {
    struct {
        uint8_t n_path_delay: 3;
        uint8_t r_path_delay: 3;
        uint8_t rab_sync_pin_reset: 2;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_4_REG_Type;

typedef union {
    struct {
        uint8_t ld_pin_control: 6;
        uint8_t ref_freq_mon_threshold: 1;
        uint8_t reserved: 1;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_5_REG_Type;

typedef union {
    struct {
        uint8_t refmon_pin_control: 5;
        uint8_t ref1_freq_mon: 1;
        uint8_t ref2_freq_mon: 1;
        uint8_t vco_freq_mon: 1;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_6_REG_Type;

typedef union {
    struct {
        uint8_t diff_ref: 1;
        uint8_t ref1_poweron: 1;
        uint8_t ref2_poweron: 1;
        uint8_t reserved: 2;
        uint8_t use_ref_sel_pin: 1;
        uint8_t select_ref2: 1;
        uint8_t disable_switchover_deglitch: 1;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_7_REG_Type;

typedef union {
    struct {
        uint8_t holdover_enable: 1;
        uint8_t external_holdover_control: 1;
        uint8_t holdover_enable_2: 1;
        uint8_t ld_pin_comp_enable: 2;
        uint8_t pll_status_reg_disable: 1;
        uint8_t reserved: 3;
    } b;
    uint8_t raw;
} AD9516_PLL_Control_8_REG_Type;

typedef struct {
    AD9516_PLL_Control_1_REG_Type pll_control_1;
    AD9516_PLL_Control_2_REG_Type pll_control_2;
    AD9516_PLL_Control_3_REG_Type pll_control_3;
    AD9516_PLL_Control_4_REG_Type pll_control_4;
    AD9516_PLL_Control_5_REG_Type pll_control_5;
    AD9516_PLL_Control_6_REG_Type pll_control_6;
    AD9516_PLL_Control_7_REG_Type pll_control_7;
    AD9516_PLL_Control_8_REG_Type pll_control_8;
} AD9516_Pll_Control;

enum {
    AD9516_REG1_CONFIG_0  = 0x0000,
    AD9516_REG1_PART_ID = 0x0003,
    AD9516_REG1_PFD_CP = 0x0010,
    AD9516_REG1_PLL_READBACK = 0x001F,
    AD9516_REG1_LVDS_CHANNEL1_DIV = 0x0199,
    AD9516_REG1_LVDS_CHANNELS_PHASE = 0x019A,
    AD9516_REG1_LVDS_CHANNEL2_DIV = 0x019B,
    AD9516_REG1_VCO_DIVIDER = 0x01E0,
    AD9516_REG1_CLOCKS = 0x01E1
};

enum {
    AD9516_PART_ID = 0xC3
};

#endif // AD9516_REGS_H
