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

#ifndef AD9545_REGS_H
#define AD9545_REGS_H

#include <stdint.h>

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
    AD9545_REG1_320E = 0x320E
};

enum {
    AD9545_VENDOR_ID = 0x0456
};

#endif // AD9545_REGS_H
