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

#include "gpio.h"
#include "i2c.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "ad9545_setup.h"
#include "ad9545_i2c_hal.h"
#include "ad9545.h"
#include "ad9545_regs.h"
#include "ad9545_status_regs.h"
#include "ad9545_print.h"
#include "ansi_escape_codes.h"
#include "logbuffer.h"

static bool pllIoUpdate(void)
{
    uint8_t data = 1;
    return ad9545_write1(0x000F, data);
}

bool pllRegisterPulseBit_unused(uint16_t address, uint8_t bitmask)
{
    uint8_t data = 0;
    return pllIoUpdate() &&
           ad9545_read1(address, &data) &&
           ad9545_write1(address, data | bitmask) &&
           pllIoUpdate() &&
           ad9545_write1(address, data & ~bitmask) &&
           pllIoUpdate();
}

bool pllCalibrateApll_unused(void)
{
    uint8_t OpControlChannel;

    // calibrate APLL 0 (requires IO Update, not autoclearing)
    OpControlChannel = 0x02; // calibrate
    if (! (ad9545_write1(AD9545_REG1_2100, OpControlChannel) &&
          pllIoUpdate()))
        return false;
    OpControlChannel = 0; // clear calibrate bit
    if (! (ad9545_write1(AD9545_REG1_2100, OpControlChannel) &&
          pllIoUpdate()))
        return false;

    // calibrate APLL 1 (requires IO Update, not autoclearing)
    OpControlChannel = 0x02; // calibrate
    if (! (ad9545_write1(AD9545_REG1_2200, OpControlChannel) &&
          pllIoUpdate()))
        return false;
    OpControlChannel = 0; // clear calibrate bit
    if (! (ad9545_write1(AD9545_REG1_2200, OpControlChannel) &&
          pllIoUpdate()))
        return false;

    return true;
}

bool pllResetOutputDividers_unused(void)
{
    return ad9545_write1(AD9545_REG1_2102, 0x1) &&
           ad9545_write1(AD9545_REG1_2103, 0x1) &&
           ad9545_write1(AD9545_REG1_2104, 0x1) &&
           ad9545_write1(AD9545_REG1_2202, 0x1) &&
           ad9545_write1(AD9545_REG1_2203, 0x1) &&
           pllIoUpdate() &&

           ad9545_write1(AD9545_REG1_2102, 0x0) &&
           ad9545_write1(AD9545_REG1_2103, 0x0) &&
           ad9545_write1(AD9545_REG1_2104, 0x0) &&
           ad9545_write1(AD9545_REG1_2202, 0x0) &&
           ad9545_write1(AD9545_REG1_2203, 0x0) &&
           pllIoUpdate();
}

bool pllClearAutomute_unused(void)
{
    return ad9545_write1(AD9545_REG1_2107, 0x10) &&
           ad9545_write1(AD9545_REG1_2207, 0x10) &&
           pllIoUpdate() &&
           ad9545_write1(AD9545_REG1_2107, 0x00) &&
           ad9545_write1(AD9545_REG1_2207, 0x00) &&
           pllIoUpdate();
}

bool pllReadAllRegisters_unused(void)
{
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
        {0x3A00,0x3A3B}
    };
    for (int n=0; n<size; n++) {
        uint16_t first = regs[n].first;
        uint16_t last = regs[n].last;
        for (int i=first; i<=last; i++) {
            uint8_t data = 0;
            if (! ad9545_read1(i, &data))
                return false;
            //            printf("0x%04X,0x%02X\n", i, data);
        }
    }
    return true;
}
