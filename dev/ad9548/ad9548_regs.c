/*
**    Copyright 2020 Ilja Slepnev
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

#include "ad9548_regs.h"

// System Clock Settings
const PLL_SysClk_regs Default_PLL_SysClk = {
    {0x0100, 0b01000001},	// Charge Pump and Lock Detect Control
    {0x0101, 0x18},			// N Divider, N from 6 to 255 (default = 0x28 = 40, current = 0x18 = 24)
    {0x0102, 0b01000101},	// SYSCLK Input Options
    {0x0103, 0x3A},			// System clock period, Bits[7:0]
    {0x0104, 0x5A},			// System clock period, Bits[15:8]
    {0x0105, 0x10},			// [4:0] System clock period, Bits[20:16]
    {0x0106, 0x01},			// System clock stability period, Bits[7:0] (default = 0x01)
    {0x0107, 0x00},			// System clock stability period, Bits[15:8] (default = 0x00)
    {0x0108, 0x00}			// System clock stability period, Bits[19:16] (default = 0x0, default period = 0x00001, or 1 ms)
};

// Multifunction Pins
const PLL_MFPins_regs Default_PLL_MFPins = {
    {0x0200, (0x01 << 7) | 64},	// M0, Reference A active
    {0x0201, (0x01 << 7) | 66},	// M1, Reference B active
    {0x0202, (0x01 << 7) | 68},	// M2, Reference C active
    {0x0203, (0x01 << 7) | 70},	// M3, Reference D active
    {0x0204, (0x01 << 7) | 7},	// M4, System Clock PLL locked
    {0x0205, (0x01 << 7) | 11},	// M5, System Clock PLL stable
    {0x0206, (0x01 << 7) | 21},	// M6, DPLL Phase locked
    {0x0207, (0x01 << 7) | 22}	// M7, DPLL Freq locked
};

// IRQ settings
const PLL_IRQ_regs Default_PLL_IRQ = {
    {0x0208, 0x00},	// IRQ Pin Output Mode
    {0x0209, 0x33},	// IRQ Mask for SYSCLK
    {0x020A, 0x0F},	// IRQ Mask for Distribution Sync, Watchdog Timer, and EEPROM
    {0x020B, 0xFF},	// IRQ Mask for the Digital PLL
    {0x020C, 0x1F},	// IRQ Mask for History Update, Frequency Limit, and Phase Slew Limit
    {0x020D, 0xFF},	// IRQ Mask for Reference Inputs (AA/A)
    {0x020E, 0xFF},	// IRQ Mask for Reference Inputs (BB/B)
    {0x020F, 0xFF},	// IRQ Mask for Reference Inputs (CC/C)
    {0x0210, 0xFF},	// IRQ Mask for Reference Inputs (DD/D)
};

// DPLL Settings
const PLL_DPLL_regs Default_PLL_DPLL = {
    {0x0300, 0x7F},		// Free running frequency tuning word, Bits[7:0]
    {0x0301, 0xEF},		// Free running frequency tuning word, Bits[15:8]
    {0x0302, 0x46},		// Free running frequency tuning word, Bits[23:9]
    {0x0303, 0x26},		// Free running frequency tuning word, Bits[31:24]
    {0x0304, 0x4B},		// Free running frequency tuning word, Bits[39:32]
    {0x0305, 0x22},		// Free running frequency tuning word, Bits[47:40]
    {0x0306, 0x00},		// Update TW
    {0x0307, 0x00},		// Lower limit pull-in range, Bits[7:0]
    {0x0308, 0x00},		// Lower limit pull-in range, Bits[15:8]
    {0x0309, 0x00},		// Lower limit pull-in range, Bits[23:9]
    {0x030A, 0xFF},		// Upper limit pull-in range, Bits[7:0]
    {0x030B, 0xFF},		// Upper limit pull-in range, Bits[15:8]
    {0x030C, 0xFF},		// Upper limit pull-in range, Bits[23:9]
    {0x030D, 0x00},		// DDS phase offset, Bits[7:0]
    {0x030E, 0x00},		// DDS phase offset, Bits[15:8]
    {0x030F, 0x00},		// Fixed phase lock offset, Bits[7:0] (0x90)
    {0x0310, 0x00},		// Fixed phase lock offset, Bits[15:8] (0x0B)
    {0x0311, 0x00},		// Fixed phase lock offset, Bits[23:16]
    {0x0312, 0x00},		// Fixed phase lock offset, Bits[31:24]
    {0x0313, 0x00},		// Fixed phase lock offset, Bits[39:32]
    {0x0314, 0xE8},		// Incremental phase lock offset step size, Bits[7:0]
    {0x0315, 0x03},		// Incremental phase lock offset step size, Bits[15:8]
    {0x0316, 0x00},		// Phase slew rate limit, Bits[7:0]
    {0x0317, 0x00},		// Phase slew rate limit, Bits[15:8]
    {0x0318, 0x30},		// History accumulation timer, Bits[7:0]
    {0x0319, 0x75},		// History accumulation timer, Bits[15:8]
    {0x031A, 0x00},		// History accumulation timer, Bits[23:16]
    {0x031B, 0x00}		// History Mode
};

const PLL_OutClk_regs PLL_OutClk_ADC64VE = {
    {0x0400, 0x00},		// Distribution Settings
    {0x0401, 0x0F},		// Distribution Enable
    {0x0402, 0x20},		// Distribution Synchronization
    {0x0403, 0x02},		// Automatic Synchronization
    {0x0404, 0x04},		// Distribution Channel Modes (OUT0)
    {0x0405, 0x04},		// Distribution Channel Modes (OUT1)
    {0x0406, 0x04},		// Distribution Channel Modes (OUT2)
    {0x0407, 0x0C},		// Distribution Channel Modes (OUT3)
    {0x0408, 0x00},		// Q0 divider, Bits[7:0]
    {0x0409, 0x00},		// Q0 divider, Bits[15:8]
    {0x040A, 0x00},		// Q0 divider, Bits[23:16]
    {0x040B, 0x00},		// [5:0] Q0 divider, Bits[29:24]
    {0x040C, 0x01},		// Q1 divider, Bits[7:0]
    {0x040D, 0x00},		// Q1 divider, Bits[15:8]
    {0x040E, 0x00},		// Q1 divider, Bits[23:16]
    {0x040F, 0x00},		// [5:0] Q1 divider, Bits[29:24]
    {0x0410, 0x01},		// Q2 divider, Bits[7:0]
    {0x0411, 0x00},		// Q2 divider, Bits[15:8]
    {0x0412, 0x00},		// Q2 divider, Bits[23:16]
    {0x0413, 0x00},		// [5:0] Q2 divider, Bits[29:24]
    {0x0414, 0x00},		// Q3 divider, Bits[7:0]
    {0x0415, 0x00},		// Q3 divider, Bits[15:8]
    {0x0416, 0x00},		// Q3 divider, Bits[23:16]
    {0x0417, 0x00}		// [5:0] Q3 divider, Bits[29:24]
};

const PLL_RefIn_regs PLL_RefIn_ADC64VE = {
    {0x0500, 0x00},			// Reference Power-Down
    {0x0501, 0x00},			// Reference Logic Family (A, AA, B, BB)
    {0x0502, 0x00},			// Reference Logic Family (C, CC, D, DD)
    {0x0503, 0b00001000},	// Manual Reference Profile Selection (AA/A) Profile 0
    {0x0504, 0b00001010},	// Manual Reference Profile Selection (BB/B) Profile 2
    {0x0505, 0b00000000},	// Manual Reference Profile Selection (CC/C) Profile 4
    {0x0506, 0b00001011},	// Manual Reference Profile Selection (DD/D) Profile 3
    {0x0507, 0x01}			// Phase Build-Out Switching
};

// Profile 0 - Dedicated to Ref A (for 125/2 MHz)
const PLL_Profile_regs PLL_Prof0_ADC64VE = {
    {0x0600, 0x1B},		// Priorities
    {0x0601, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0602, 0x24},		// Nominal reference period, Bits[15:8]
    {0x0603, 0xF4},		// Nominal reference period, Bits[23:16]
    {0x0604, 0x00},		// Nominal reference period, Bits[31:24]
    {0x0605, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0606, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0607, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0608, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0609, 0x03},		// Inner tolerance, Bits[15:8]
    {0x060A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x060B, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x060C, 0x03},		// Outer tolerance, Bits[5:8]
    {0x060D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x060E, 0x0A},		// Validation timer, Bits[7:0]
    {0x060F, 0x00},		// Validation timer, Bits[15:8]
    {0x0610, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0611, 0x00},		// Redetect timer, Bits[15:8]
    {0x0612, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0613, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0614, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0615, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0616, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0617, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0618, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0619, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x061A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x061B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x061C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x061D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x061E, 0x1D},		// R, Bits[7:0]
    {0x061F, 0x00},		// R, Bits[15:8]
    {0x0620, 0x00},		// R, Bits[23:16]
    {0x0621, 0x00},		// [5:0] R, Bits[29:24]
    {0x0622, 0x3B},		// S, Bits[7:0]
    {0x0623, 0x00},		// S, Bits[15:8]
    {0x0624, 0x00},		// S, Bits[23:16]
    {0x0625, 0x00},		// [5:0] S, Bits[29:24]
    {0x0626, 0x00},		// V, Bits[7:0]
    {0x0627, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x0628, 0x00},		// [5:0] U, Bits[9:4]
    {0x0629, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x062A, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x062B, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x062C, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x062D, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x062E, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x062F, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0630, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0631, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 1 - Dedicated to Ref B (for 125/3 MHz)
const PLL_Profile_regs PLL_Prof1_ADC64VE = {
    {0x0632, 0x00},		// Priorities
    {0x0633, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0634, 0x36},		// Nominal reference period, Bits[15:8]
    {0x0635, 0x6E},		// Nominal reference period, Bits[23:16]
    {0x0636, 0x01},		// Nominal reference period, Bits[31:24]
    {0x0637, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0638, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0639, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x063A, 0xC8},		// Inner tolerance, Bits[7:0]
    {0x063B, 0x00},		// Inner tolerance, Bits[15:8]
    {0x063C, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x063D, 0x0A},		// Outer tolerance, Bits[7:0]
    {0x063E, 0x00},		// Outer tolerance, Bits[5:8]
    {0x063F, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x0640, 0x0A},		// Validation timer, Bits[7:0]
    {0x0641, 0x00},		// Validation timer, Bits[15:8]
    {0x0642, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0643, 0x00},		// Redetect timer, Bits[15:8]
    {0x0644, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0645, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0646, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0647, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0648, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0649, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x064A, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x064B, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x064C, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x064D, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x064E, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x064F, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x0650, 0x13},		// R, Bits[7:0]
    {0x0651, 0x00},		// R, Bits[15:8]
    {0x0652, 0x00},		// R, Bits[23:16]
    {0x0653, 0x00},		// [5:0] R, Bits[29:24]
    {0x0654, 0x3B},		// S, Bits[7:0]
    {0x0655, 0x00},		// S, Bits[15:8]
    {0x0656, 0x00},		// S, Bits[23:16]
    {0x0657, 0x00},		// [5:0] S, Bits[29:24]
    {0x0658, 0x00},		// V, Bits[7:0]
    {0x0659, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x065A, 0x00},		// [5:0] U, Bits[9:4]
    {0x065B, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x065C, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x065D, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x065E, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x065F, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x0660, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x0661, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0662, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0663, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 2 - Dedicated to Ref B (for 41.7/2 MHz)
const PLL_Profile_regs PLL_Prof2_ADC64VE = {
    {0x0680, 0x00},		// Priorities
    {0x0681, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0682, 0x6C},		// Nominal reference period, Bits[15:8]
    {0x0683, 0xDC},		// Nominal reference period, Bits[23:16]
    {0x0684, 0x02},		// Nominal reference period, Bits[31:24]
    {0x0685, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0686, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0687, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0688, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0689, 0x03},		// Inner tolerance, Bits[15:8]
    {0x068A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x068B, 0xC8},		// Outer tolerance, Bits[7:0]
    {0x068C, 0x00},		// Outer tolerance, Bits[5:8]
    {0x068D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x068E, 0x0A},		// Validation timer, Bits[7:0]
    {0x068F, 0x00},		// Validation timer, Bits[15:8]
    {0x0690, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0691, 0x00},		// Redetect timer, Bits[15:8]
    {0x0692, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0693, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0694, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0695, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0696, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0697, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0698, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0699, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x069A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x069B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x069C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x069D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x069E, 0x09},		// R, Bits[7:0]
    {0x069F, 0x00},		// R, Bits[15:8]
    {0x06A0, 0x00},		// R, Bits[23:16]
    {0x06A1, 0x00},		// [5:0] R, Bits[29:24]
    {0x06A2, 0x3B},		// S, Bits[7:0]
    {0x06A3, 0x00},		// S, Bits[15:8]
    {0x06A4, 0x00},		// S, Bits[23:16]
    {0x06A5, 0x00},		// [5:0] S, Bits[29:24]
    {0x06A6, 0x00},		// V, Bits[7:0]
    {0x06A7, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06A8, 0x00},		// [5:0] U, Bits[9:4]
    {0x06A9, 0x10},		// Phase lock threshold, Bits[7:0]
    {0x06AA, 0x27},		// Phase lock threshold, Bits[15:8]
    {0x06AB, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06AC, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06AD, 0xF4},		// Frequency lock threshold, Bits[7:0]
    {0x06AE, 0x01},		// Frequency lock threshold, Bits[15:8]
    {0x06AF, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06B0, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06B1, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 3 - Dedicated to Ref D (for 125/2 MHz)
const PLL_Profile_regs PLL_Prof3_ADC64VE = {
    {0x06B2, 0x09},		// Priorities
    {0x06B3, 0x00},		// Nominal reference period, Bits[7:0]
    {0x06B4, 0x24},		// Nominal reference period, Bits[15:8]
    {0x06B5, 0xF4},		// Nominal reference period, Bits[23:16]
    {0x06B6, 0x00},		// Nominal reference period, Bits[31:24]
    {0x06B7, 0x00},		// Nominal reference period, Bits[39:32]
    {0x06B8, 0x00},		// Nominal reference period, Bits[47:40]
    {0x06B9, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x06BA, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x06BB, 0x03},		// Inner tolerance, Bits[15:8]
    {0x06BC, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x06BD, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x06BE, 0x03},		// Outer tolerance, Bits[5:8]
    {0x06BF, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x06C0, 0x0A},		// Validation timer, Bits[7:0]
    {0x06C1, 0x00},		// Validation timer, Bits[15:8]
    {0x06C2, 0x0A},		// Redetect timer, Bits[7:0]
    {0x06C3, 0x00},		// Redetect timer, Bits[15:8]
    {0x06C4, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x06C5, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x06C6, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x06C7, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x06C8, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x06C9, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x06CA, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x06CB, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x06CC, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x06CD, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x06CE, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x06CF, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x06D0, 0x1D},		// R, Bits[7:0]
    {0x06D1, 0x00},		// R, Bits[15:8]
    {0x06D2, 0x00},		// R, Bits[23:16]
    {0x06D3, 0x00},		// [5:0] R, Bits[29:24]
    {0x06D4, 0x3B},		// S, Bits[7:0]
    {0x06D5, 0x00},		// S, Bits[15:8]
    {0x06D6, 0x00},		// S, Bits[23:16]
    {0x06D7, 0x00},		// [5:0] S, Bits[29:24]
    {0x06D8, 0x00},		// V, Bits[7:0]
    {0x06D9, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06DA, 0x00},		// [5:0] U, Bits[9:4]
    {0x06DB, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x06DC, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x06DD, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06DE, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06DF, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x06E0, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x06E1, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06E2, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06E3, 0x44}		// Frequency lock drain rate, Bits[7:0]
};


// Clock Distribution Output Settings
const PLL_OutClk_regs PLL_OutClk_TDC_VHLE = {
    {0x0400, 0x00},		// Distribution Settings
    {0x0401, 0x0F},		// Distribution Enable
    {0x0402, 0x10},		// Distribution Synchronization
    {0x0403, 0x02},		// Automatic Synchronization
    {0x0404, 0x04},		// Distribution Channel Modes (OUT0)
    {0x0405, 0x04},		// Distribution Channel Modes (OUT1)
    {0x0406, 0x04},		// Distribution Channel Modes (OUT2)
    {0x0407, 0x0C},		// Distribution Channel Modes (OUT3)
    {0x0408, 0x00},		// Q0 divider, Bits[7:0]
    {0x0409, 0x00},		// Q0 divider, Bits[15:8]
    {0x040A, 0x00},		// Q0 divider, Bits[23:16]
    {0x040B, 0x00},		// [5:0] Q0 divider, Bits[29:24]
    {0x040C, 0x02},		// Q1 divider, Bits[7:0]
    {0x040D, 0x00},		// Q1 divider, Bits[15:8]
    {0x040E, 0x00},		// Q1 divider, Bits[23:16]
    {0x040F, 0x00},		// [5:0] Q1 divider, Bits[29:24]
    {0x0410, 0x00},		// Q2 divider, Bits[7:0]
    {0x0411, 0x00},		// Q2 divider, Bits[15:8]
    {0x0412, 0x00},		// Q2 divider, Bits[23:16]
    {0x0413, 0x00},		// [5:0] Q2 divider, Bits[29:24]
    {0x0414, 0x00},		// Q3 divider, Bits[7:0]
    {0x0415, 0x00},		// Q3 divider, Bits[15:8]
    {0x0416, 0x00},		// Q3 divider, Bits[23:16]
    {0x0417, 0x00}		// [5:0] Q3 divider, Bits[29:24]
};

// Reference Inputs Settings
const PLL_RefIn_regs PLL_RefIn_TDC_VHLE = {
    {0x0500, 0x03},			// Reference Power-Down
    {0x0501, 0x00},			// Reference Logic Family (A, AA, B, BB)
    {0x0502, 0x00},			// Reference Logic Family (C, CC, D, DD)
    {0x0503, 0b00001000},	// Manual Reference Profile Selection (AA/A)
    {0x0504, 0b00001001},	// Manual Reference Profile Selection (BB/B)
    {0x0505, 0b00000000},	// Manual Reference Profile Selection (CC/C)
    {0x0506, 0b00001011},	// Manual Reference Profile Selection (DD/D)
    {0x0507, 0x01}			// Phase Build-Out Switching
};

// Profile 0 Settings
const PLL_Profile_regs PLL_Prof0_TDC_VHLE = {
    {0x0600, 0x1B},		// Priorities
    {0x0601, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0602, 0x12},		// Nominal reference period, Bits[15:8]
    {0x0603, 0x7A},		// Nominal reference period, Bits[23:16]
    {0x0604, 0x00},		// Nominal reference period, Bits[31:24]
    {0x0605, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0606, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0607, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0608, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0609, 0x03},		// Inner tolerance, Bits[15:8]
    {0x060A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x060B, 0xC8},		// Outer tolerance, Bits[7:0]
    {0x060C, 0x00},		// Outer tolerance, Bits[5:8]
    {0x060D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x060E, 0xD0},		// Validation timer, Bits[7:0]
    {0x060F, 0x07},		// Validation timer, Bits[15:8]
    {0x0610, 0xD0},		// Redetect timer, Bits[7:0]
    {0x0611, 0x07},		// Redetect timer, Bits[15:8]
    {0x0612, 0xCC},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0613, 0xCE},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0614, 0x07},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0615, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0616, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0617, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0618, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0619, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x061A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x061B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x061C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x061D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x061E, 0x13},		// R, Bits[7:0]
    {0x061F, 0x00},		// R, Bits[15:8]
    {0x0620, 0x00},		// R, Bits[23:16]
    {0x0621, 0x00},		// [5:0] R, Bits[29:24]
    {0x0622, 0x13},		// S, Bits[7:0]
    {0x0623, 0x00},		// S, Bits[15:8]
    {0x0624, 0x00},		// S, Bits[23:16]
    {0x0625, 0x00},		// [5:0] S, Bits[29:24]
    {0x0626, 0x00},		// V, Bits[7:0]
    {0x0627, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x0628, 0x00},		// [5:0] U, Bits[9:4]
    {0x0629, 0x10},		// Phase lock threshold, Bits[7:0]
    {0x062A, 0x27},		// Phase lock threshold, Bits[15:8]
    {0x062B, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x062C, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x062D, 0xF4},		// Frequency lock threshold, Bits[7:0]
    {0x062E, 0x01},		// Frequency lock threshold, Bits[15:8]
    {0x062F, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0630, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0631, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 1 Settings
const PLL_Profile_regs PLL_Prof1_TDC_VHLE = {
    {0x0632, 0x00},		// Priorities
    {0x0633, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0634, 0x36},		// Nominal reference period, Bits[15:8]
    {0x0635, 0x6E},		// Nominal reference period, Bits[23:16]
    {0x0636, 0x01},		// Nominal reference period, Bits[31:24]
    {0x0637, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0638, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0639, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x063A, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x063B, 0x03},		// Inner tolerance, Bits[15:8]
    {0x063C, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x063D, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x063E, 0x03},		// Outer tolerance, Bits[5:8]
    {0x063F, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x0640, 0x0A},		// Validation timer, Bits[7:0]
    {0x0641, 0x00},		// Validation timer, Bits[15:8]
    {0x0642, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0643, 0x00},		// Redetect timer, Bits[15:8]
    {0x0644, 0x57},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0645, 0xF2},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0646, 0x00},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0647, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0648, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0649, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x064A, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x064B, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x064C, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x064D, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x064E, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x064F, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x0650, 0xE7},		// R, Bits[7:0]
    {0x0651, 0x03},		// R, Bits[15:8]
    {0x0652, 0x00},		// R, Bits[23:16]
    {0x0653, 0x00},		// [5:0] R, Bits[29:24]
    {0x0654, 0xB7},		// S, Bits[7:0]
    {0x0655, 0x0B},		// S, Bits[15:8]
    {0x0656, 0x00},		// S, Bits[23:16]
    {0x0657, 0x00},		// [5:0] S, Bits[29:24]
    {0x0658, 0x00},		// V, Bits[7:0]
    {0x0659, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x065A, 0x00},		// [5:0] U, Bits[9:4]
    {0x065B, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x065C, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x065D, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x065E, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x065F, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x0660, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x0661, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0662, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0663, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 2 - Dedicated to Ref B (40 MHz)
const PLL_Profile_regs PLL_Prof2_TDC_VHLE = {
    {0x0680, 0x00},		// Priorities
    {0x0681, 0x40},		// Nominal reference period, Bits[7:0]
    {0x0682, 0x78},		// Nominal reference period, Bits[15:8]
    {0x0683, 0x7D},		// Nominal reference period, Bits[23:16]
    {0x0684, 0x01},		// Nominal reference period, Bits[31:24]
    {0x0685, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0686, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0687, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0688, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0689, 0x03},		// Inner tolerance, Bits[15:8]
    {0x068A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x068B, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x068C, 0x03},		// Outer tolerance, Bits[5:8]
    {0x068D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x068E, 0x0A},		// Validation timer, Bits[7:0]
    {0x068F, 0x00},		// Validation timer, Bits[15:8]
    {0x0690, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0691, 0x00},		// Redetect timer, Bits[15:8]
    {0x0692, 0x3F},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0693, 0x81},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0694, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0695, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0696, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0697, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0698, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0699, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x069A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x069B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x069C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x069D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x069E, 0x0F},		// R, Bits[7:0]
    {0x069F, 0x00},		// R, Bits[15:8]
    {0x06A0, 0x00},		// R, Bits[23:16]
    {0x06A1, 0x00},		// [5:0] R, Bits[29:24]
    {0x06A2, 0x31},		// S, Bits[7:0]
    {0x06A3, 0x00},		// S, Bits[15:8]
    {0x06A4, 0x00},		// S, Bits[23:16]
    {0x06A5, 0x00},		// [5:0] S, Bits[29:24]
    {0x06A6, 0x00},		// V, Bits[7:0]
    {0x06A7, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06A8, 0x00},		// [5:0] U, Bits[9:4]
    {0x06A9, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x06AA, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x06AB, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06AC, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06AD, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x06AE, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x06AF, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06B0, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06B1, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 3 Settings
const PLL_Profile_regs PLL_Prof3_TDC_VHLE = {
    {0x06B2, 0x09},		// Priorities
    {0x06B3, 0x00},		// Nominal reference period, Bits[7:0]
    {0x06B4, 0x36},		// Nominal reference period, Bits[15:8]
    {0x06B5, 0x6E},		// Nominal reference period, Bits[23:16]
    {0x06B6, 0x01},		// Nominal reference period, Bits[31:24]
    {0x06B7, 0x00},		// Nominal reference period, Bits[39:32]
    {0x06B8, 0x00},		// Nominal reference period, Bits[47:40]
    {0x06B9, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x06BA, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x06BB, 0x03},		// Inner tolerance, Bits[15:8]
    {0x06BC, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x06BD, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x06BE, 0x03},		// Outer tolerance, Bits[5:8]
    {0x06BF, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x06C0, 0x0A},		// Validation timer, Bits[7:0]
    {0x06C1, 0x00},		// Validation timer, Bits[15:8]
    {0x06C2, 0x0A},		// Redetect timer, Bits[7:0]
    {0x06C3, 0x00},		// Redetect timer, Bits[15:8]
    {0x06C4, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x06C5, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x06C6, 0x06},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x06C7, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x06C8, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x06C9, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x06CA, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x06CB, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x06CC, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x06CD, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x06CE, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x06CF, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x06D0, 0x09},		// R, Bits[7:0]
    {0x06D1, 0x00},		// R, Bits[15:8]
    {0x06D2, 0x00},		// R, Bits[23:16]
    {0x06D3, 0x00},		// [5:0] R, Bits[29:24]
    {0x06D4, 0x1D},		// S, Bits[7:0]
    {0x06D5, 0x00},		// S, Bits[15:8]
    {0x06D6, 0x00},		// S, Bits[23:16]
    {0x06D7, 0x00},		// [5:0] S, Bits[29:24]
    {0x06D8, 0x00},		// V, Bits[7:0]
    {0x06D9, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06DA, 0x00},		// [5:0] U, Bits[9:4]
    {0x06DB, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x06DC, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x06DD, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06DE, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06DF, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x06E0, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x06E1, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06E2, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06E3, 0x44}		// Frequency lock drain rate, Bits[7:0]
};



// Clock Distribution Output Settings
const PLL_OutClk_regs PLL_OutClk_TQDC16VS = {
    {0x0400, 0x00},		// Distribution Settings
    {0x0401, 0x0F},		// Distribution Enable
    {0x0402, 0x10},		// Distribution Synchronization
    {0x0403, 0x02},		// Automatic Synchronization
    {0x0404, 0x04},		// Distribution Channel Modes (OUT0)
    {0x0405, 0x04},		// Distribution Channel Modes (OUT1)
    {0x0406, 0x04},		// Distribution Channel Modes (OUT2)
    {0x0407, 0x0C},		// Distribution Channel Modes (OUT3)
    {0x0408, 0x00},		// Q0 divider, Bits[7:0]
    {0x0409, 0x00},		// Q0 divider, Bits[15:8]
    {0x040A, 0x00},		// Q0 divider, Bits[23:16]
    {0x040B, 0x00},		// [5:0] Q0 divider, Bits[29:24]
    {0x040C, 0x02},		// Q1 divider, Bits[7:0]
    {0x040D, 0x00},		// Q1 divider, Bits[15:8]
    {0x040E, 0x00},		// Q1 divider, Bits[23:16]
    {0x040F, 0x00},		// [5:0] Q1 divider, Bits[29:24]
    {0x0410, 0x00},		// Q2 divider, Bits[7:0]
    {0x0411, 0x00},		// Q2 divider, Bits[15:8]
    {0x0412, 0x00},		// Q2 divider, Bits[23:16]
    {0x0413, 0x00},		// [5:0] Q2 divider, Bits[29:24]
    {0x0414, 0x00},		// Q3 divider, Bits[7:0]
    {0x0415, 0x00},		// Q3 divider, Bits[15:8]
    {0x0416, 0x00},		// Q3 divider, Bits[23:16]
    {0x0417, 0x00}		// [5:0] Q3 divider, Bits[29:24]
};

// Reference Inputs Settings
const PLL_RefIn_regs PLL_RefIn_TQDC16VS = {
    {0x0500, 0x03},			// Reference Power-Down
    {0x0501, 0x00},			// Reference Logic Family (A, AA, B, BB)
    {0x0502, 0x00},			// Reference Logic Family (C, CC, D, DD)
    {0x0503, 0b00001000},	// Manual Reference Profile Selection (AA/A)
    {0x0504, 0b00000001},	// Manual Reference Profile Selection (BB/B)
    {0x0505, 0b00000000},	// Manual Reference Profile Selection (CC/C)
    {0x0506, 0b00001011},	// Manual Reference Profile Selection (DD/D)
    {0x0507, 0x01}			// Phase Build-Out Switching
};

// Profile 0 - Dedicated to Ref A
const PLL_Profile_regs PLL_Prof0_TQDC16VS = {
    {0x0600, 0x1B},		// Priorities
    {0x0601, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0602, 0x12},		// Nominal reference period, Bits[15:8]
    {0x0603, 0x7A},		// Nominal reference period, Bits[23:16]
    {0x0604, 0x00},		// Nominal reference period, Bits[31:24]
    {0x0605, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0606, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0607, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0608, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0609, 0x03},		// Inner tolerance, Bits[15:8]
    {0x060A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x060B, 0xC8},		// Outer tolerance, Bits[7:0]
    {0x060C, 0x00},		// Outer tolerance, Bits[5:8]
    {0x060D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x060E, 0xD0},		// Validation timer, Bits[7:0]
    {0x060F, 0x07},		// Validation timer, Bits[15:8]
    {0x0610, 0xD0},		// Redetect timer, Bits[7:0]
    {0x0611, 0x07},		// Redetect timer, Bits[15:8]
    {0x0612, 0xCC},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0613, 0xCE},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0614, 0x07},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0615, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0616, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0617, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0618, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0619, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x061A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x061B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x061C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x061D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x061E, 0x13},		// R, Bits[7:0]
    {0x061F, 0x00},		// R, Bits[15:8]
    {0x0620, 0x00},		// R, Bits[23:16]
    {0x0621, 0x00},		// [5:0] R, Bits[29:24]
    {0x0622, 0x13},		// S, Bits[7:0]
    {0x0623, 0x00},		// S, Bits[15:8]
    {0x0624, 0x00},		// S, Bits[23:16]
    {0x0625, 0x00},		// [5:0] S, Bits[29:24]
    {0x0626, 0x00},		// V, Bits[7:0]
    {0x0627, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x0628, 0x00},		// [5:0] U, Bits[9:4]
    {0x0629, 0x10},		// Phase lock threshold, Bits[7:0]
    {0x062A, 0x27},		// Phase lock threshold, Bits[15:8]
    {0x062B, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x062C, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x062D, 0xF4},		// Frequency lock threshold, Bits[7:0]
    {0x062E, 0x01},		// Frequency lock threshold, Bits[15:8]
    {0x062F, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0630, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0631, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 1 - Dedicated to Ref B (41.67 MHz)
const PLL_Profile_regs PLL_Prof1_TQDC16VS = {
    {0x0632, 0x00},		// Priorities
    {0x0633, 0x00},		// Nominal reference period, Bits[7:0]
    {0x0634, 0x36},		// Nominal reference period, Bits[15:8]
    {0x0635, 0x6E},		// Nominal reference period, Bits[23:16]
    {0x0636, 0x01},		// Nominal reference period, Bits[31:24]
    {0x0637, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0638, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0639, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x063A, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x063B, 0x03},		// Inner tolerance, Bits[15:8]
    {0x063C, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x063D, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x063E, 0x03},		// Outer tolerance, Bits[5:8]
    {0x063F, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x0640, 0x0A},		// Validation timer, Bits[7:0]
    {0x0641, 0x00},		// Validation timer, Bits[15:8]
    {0x0642, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0643, 0x00},		// Redetect timer, Bits[15:8]
    {0x0644, 0x57},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0645, 0xF2},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0646, 0x00},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0647, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0648, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0649, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x064A, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x064B, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x064C, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x064D, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x064E, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x064F, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x0650, 0xE7},		// R, Bits[7:0]
    {0x0651, 0x03},		// R, Bits[15:8]
    {0x0652, 0x00},		// R, Bits[23:16]
    {0x0653, 0x00},		// [5:0] R, Bits[29:24]
    {0x0654, 0xB7},		// S, Bits[7:0]
    {0x0655, 0x0B},		// S, Bits[15:8]
    {0x0656, 0x00},		// S, Bits[23:16]
    {0x0657, 0x00},		// [5:0] S, Bits[29:24]
    {0x0658, 0x00},		// V, Bits[7:0]
    {0x0659, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x065A, 0x00},		// [5:0] U, Bits[9:4]
    {0x065B, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x065C, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x065D, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x065E, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x065F, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x0660, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x0661, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x0662, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x0663, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 2 - Dedicated to Ref B (40 MHz)
const PLL_Profile_regs PLL_Prof2_TQDC16VS = {
    {0x0680, 0x00},		// Priorities
    {0x0681, 0x40},		// Nominal reference period, Bits[7:0]
    {0x0682, 0x78},		// Nominal reference period, Bits[15:8]
    {0x0683, 0x7D},		// Nominal reference period, Bits[23:16]
    {0x0684, 0x01},		// Nominal reference period, Bits[31:24]
    {0x0685, 0x00},		// Nominal reference period, Bits[39:32]
    {0x0686, 0x00},		// Nominal reference period, Bits[47:40]
    {0x0687, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x0688, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x0689, 0x03},		// Inner tolerance, Bits[15:8]
    {0x068A, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x068B, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x068C, 0x03},		// Outer tolerance, Bits[5:8]
    {0x068D, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x068E, 0x0A},		// Validation timer, Bits[7:0]
    {0x068F, 0x00},		// Validation timer, Bits[15:8]
    {0x0690, 0x0A},		// Redetect timer, Bits[7:0]
    {0x0691, 0x00},		// Redetect timer, Bits[15:8]
    {0x0692, 0x3F},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x0693, 0x81},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x0694, 0x05},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x0695, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x0696, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x0697, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x0698, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x0699, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x069A, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x069B, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x069C, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x069D, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x069E, 0x0F},		// R, Bits[7:0]
    {0x069F, 0x00},		// R, Bits[15:8]
    {0x06A0, 0x00},		// R, Bits[23:16]
    {0x06A1, 0x00},		// [5:0] R, Bits[29:24]
    {0x06A2, 0x31},		// S, Bits[7:0]
    {0x06A3, 0x00},		// S, Bits[15:8]
    {0x06A4, 0x00},		// S, Bits[23:16]
    {0x06A5, 0x00},		// [5:0] S, Bits[29:24]
    {0x06A6, 0x00},		// V, Bits[7:0]
    {0x06A7, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06A8, 0x00},		// [5:0] U, Bits[9:4]
    {0x06A9, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x06AA, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x06AB, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06AC, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06AD, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x06AE, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x06AF, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06B0, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06B1, 0x44}		// Frequency lock drain rate, Bits[7:0]
};

// Profile 3 - Dedicated to Ref D
const PLL_Profile_regs PLL_Prof3_TQDC16VS = {
    {0x06B2, 0x09},		// Priorities
    {0x06B3, 0x00},		// Nominal reference period, Bits[7:0]
    {0x06B4, 0x36},		// Nominal reference period, Bits[15:8]
    {0x06B5, 0x6E},		// Nominal reference period, Bits[23:16]
    {0x06B6, 0x01},		// Nominal reference period, Bits[31:24]
    {0x06B7, 0x00},		// Nominal reference period, Bits[39:32]
    {0x06B8, 0x00},		// Nominal reference period, Bits[47:40]
    {0x06B9, 0x00},		// [1:0] Nominal reference period, Bits[49:48]
    {0x06BA, 0xE8},		// Inner tolerance, Bits[7:0]
    {0x06BB, 0x03},		// Inner tolerance, Bits[15:8]
    {0x06BC, 0x00},		// [3:0] Inner tolerance, Bits[19:16]
    {0x06BD, 0xE8},		// Outer tolerance, Bits[7:0]
    {0x06BE, 0x03},		// Outer tolerance, Bits[5:8]
    {0x06BF, 0x00},		// [3:0] Outer tolerance, Bits[19:16]
    {0x06C0, 0x0A},		// Validation timer, Bits[7:0]
    {0x06C1, 0x00},		// Validation timer, Bits[15:8]
    {0x06C2, 0x0A},		// Redetect timer, Bits[7:0]
    {0x06C3, 0x00},		// Redetect timer, Bits[15:8]
    {0x06C4, 0x19},		// Alpha-0 coefficient linear, Bits[7:0]
    {0x06C5, 0x9B},		// Alpha-0 coefficient linear, Bits[15:8]
    {0x06C6, 0x06},		// [7:6] Alpha-2 coefficient exponent, Bits[1:0]; [5:0] Alpha-1 coefficient exponent, Bits[5:0]
    {0x06C7, 0x30},		// [7:1] Beta-0 coefficient linear, Bits[6:0]; [0] Alpha-2 coefficient exponent, Bit 2
    {0x06C8, 0x80},		// Beta-0 coefficient linear, Bits[14:7]
    {0x06C9, 0x36},		// [6:2] Beta-1 coefficient exponent, Bits[4:0]; [1:0] Beta-0 coefficient linear, Bits[16:15]
    {0x06CA, 0x4A},		// Gamma-0 coefficient linear, Bits[7:0]
    {0x06CB, 0x51},		// Gamma-0 coefficient linear, Bits[15:8]
    {0x06CC, 0x1B},		// [5:1] Gamma-1 coefficient exponent, Bits[4:0]; [0] Gamma-0 coefficient linear, Bit 16
    {0x06CD, 0x3D},		// Delta-0 coefficient linear, Bits[7:0]
    {0x06CE, 0x71},		// [7] Delta-1 coefficient exponent, Bit 0; [6:0] Delta-0 coefficient linear, Bits[14:8]
    {0x06CF, 0x06},		// [7:4] Alpha-3 coefficient exponent, Bits[3:0]; [3:0] Delta-1 coefficient exponent, Bits[4:1]
    {0x06D0, 0x09},		// R, Bits[7:0]
    {0x06D1, 0x00},		// R, Bits[15:8]
    {0x06D2, 0x00},		// R, Bits[23:16]
    {0x06D3, 0x00},		// [5:0] R, Bits[29:24]
    {0x06D4, 0x1D},		// S, Bits[7:0]
    {0x06D5, 0x00},		// S, Bits[15:8]
    {0x06D6, 0x00},		// S, Bits[23:16]
    {0x06D7, 0x00},		// [5:0] S, Bits[29:24]
    {0x06D8, 0x00},		// V, Bits[7:0]
    {0x06D9, 0x00},		// [7:4] U, Bits[3:0]; [1:0] V, Bits[9:8]
    {0x06DA, 0x00},		// [5:0] U, Bits[9:4]
    {0x06DB, 0x64},		// Phase lock threshold, Bits[7:0]
    {0x06DC, 0x00},		// Phase lock threshold, Bits[15:8]
    {0x06DD, 0x20},		// Phase lock fill rate, Bits[7:0]
    {0x06DE, 0x44},		// Phase lock drain rate, Bits[7:0]
    {0x06DF, 0x64},		// Frequency lock threshold, Bits[7:0]
    {0x06E0, 0x00},		// Frequency lock threshold, Bits[15:8]
    {0x06E1, 0x00},		// Frequency lock threshold, Bits[23:16]
    {0x06E2, 0x20},		// Frequency lock fill rate, Bits[7:0]
    {0x06E3, 0x44}		// Frequency lock drain rate, Bits[7:0]
};
