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

#include "dev_auxpll.h"

#include <assert.h>

#include "ad9516/ad9516_spi_hal.h"
#include "ad9516/ad9516_status.h"
#include "ad9516/ad9516_regs.h"
#include "ansi_escape_codes.h"
#include "cmsis_os.h"
#include "dev_auxpll_types.h"
#include "log/log.h"

static bool pllIoUpdate(Dev_auxpll *d)
{
    uint8_t data = 1;
    return ad9516_write1(&d->dev.bus, 0x232, data);
}

bool auxpllSoftwareReset(Dev_auxpll *d)
{
    BusInterface *bus = &d->dev.bus;
    bool ret = true;
    volatile AD9516_Serial_Config_REG_Type serial_config_reg;
    serial_config_reg.raw = 0;
    serial_config_reg.b.long_instr = 1;
    serial_config_reg.b.long_instr_2 = 1;
    serial_config_reg.b.sdo_active = 1;
    serial_config_reg.b.sdo_active_2 = 1;
    if (! ad9516_write_config(bus, serial_config_reg.raw))
        ret = false;
    serial_config_reg.b.softreset = 1;
    serial_config_reg.b.softreset_2 = 1;
    if (! ad9516_write_config(bus, serial_config_reg.raw))
        ret = false;
    serial_config_reg.b.softreset = 0;
    serial_config_reg.b.softreset_2 = 0;
    if (! ad9516_write_config(bus, serial_config_reg.raw))
        ret = false;
    if (!ret)
        return false;
    uint8_t data = 0;
    if (! ad9516_read1(bus, AD9516_REG1_CONFIG_0, &data))
        return false;
    if (data != serial_config_reg.raw) {
        log_printf(LOG_WARNING, "AD9516: wrote %02X, read %02X", serial_config_reg.raw, data);
        return false;
    }
    return true;
}

DeviceStatus auxpllDetect(Dev_auxpll *d)
{
    BusInterface *bus = &d->dev.bus;
    int devicePresent = 0;
    uint8_t data = 0;
    if (! ad9516_read1(bus, AD9516_REG1_PART_ID, &data))
        goto err;
    devicePresent = (data == AD9516_PART_ID);
    if (devicePresent)
        log_put(LOG_INFO, "AUXPLL AD9516 present");
    d->dev.device_status = devicePresent ? DEVICE_NORMAL : DEVICE_FAIL;
    // ad9516_test_loop(); // FIXME
    return d->dev.device_status;
err:
    d->dev.device_status = DEVICE_FAIL;
    return d->dev.device_status;
}

bool auxpllReadStatus(Dev_auxpll *d)
{
    BusInterface *bus = &d->dev.bus;
    uint8_t data = 0;
    if (! ad9516_read1(bus, AD9516_REG1_PART_ID, &data))
        return false;
    if (data != AD9516_PART_ID)
        return false;

    if (!pllIoUpdate(d))
        return false;
    if (! ad9516_read1(bus, AD9516_REG1_PLL_READBACK, &d->priv.status.pll_readback.raw))
        return false;

    return true;
}

static bool auxpllReadAllRegisters_unused(Dev_auxpll *d)
{
    BusInterface *bus = &d->dev.bus;
    typedef struct {
        uint16_t first;
        uint16_t last;
    } region_t;
    enum {size = 7};
    region_t regs[size] = {
        {0x0000,0x0004},
        {0x0010,0x001F},
        {0x00A0,0x00AB},
        {0x00F0,0x00F5},
        {0x0140,0x0143},
        {0x0190,0x01A2},
        {0x01E0,0x01E1},
    };
    for (int n=0; n<size; n++) {
        uint16_t first = regs[n].first;
        uint16_t last = regs[n].last;
        for (int i=first; i<=last; i++) {
            uint8_t data = 0;
            if (! ad9516_read1(bus, i, &data))
                return false;
            // log_printf(LOG_DEBUG, "0x%04X,0x%02X", i, data);
        }
    }
    return true;
}

static bool auxpll_output_setup(Dev_auxpll *d)
{
    BusInterface *bus = &d->dev.bus;
    // output drivers
    ad9516_write1(bus, 0x140, d->priv.enable_out_6 ? 0x42 : 0x43); // OUT6
    ad9516_write1(bus, 0x141, d->priv.enable_out_7 ? 0x42 : 0x43); // OUT7
    ad9516_write1(bus, 0x142, d->priv.enable_out_8 ? 0x42 : 0x43); // OUT8, Enable, LVDS
    ad9516_write1(bus, 0x143, d->priv.enable_out_9 ? 0x42 : 0x43); // OUT9

    // output dividers
    ad9516_write1(bus, 0x199, 0);
    ad9516_write1(bus, 0x19A, 0);
    ad9516_write1(bus, 0x19B, 0);
    ad9516_write1(bus, 0x19C, 0x20); // bypass 3.2
    ad9516_write1(bus, 0x19A, 0);
    ad9516_write1(bus, 0x19E, 0);
    ad9516_write1(bus, 0x19F, 0);
    ad9516_write1(bus, 0x1A0, 0);
    ad9516_write1(bus, 0x1A1, 0x20); // bypass 4.2

    pllIoUpdate(d);
    return true;
}

//const int auxpll_vco_div_map[8] = {2, 3, 4, 5, 6, 0, 0, 0};
//const double auxpll_prescalerdiv_map[8] = {1, 2, 2./3., 4./5., 8./9., 16./17., 32./33., 3};
//const double AUXPLL_REF_FREQ = 20e6;
const int AUXPLL_REF_DIV = 1; // 1..16383
const int AUXPLL_VCO_DIV_INDEX = 4; // 4: divide by 6
const int AUXPLL_ACOUNTER = 3; //
const int AUXPLL_BCOUNTER = 9;
const int AUXPLL_PRESCALER_INDEX = 4;
const int AUXPLL_DIST_DIV = 2;
// r 1, p 8, a 3, b 9, vco_div 6, out_div 2, pfd 2e+07

bool auxpllSetup(Dev_auxpll *d)
{
//    volatile const int AUXPLL_PRESCALER_DIV = auxpll_prescalerdiv_map[AUXPLL_PRESCALER];
//    volatile const double AUXPLL_VCO_FREQ =
//        AUXPLL_REF_FREQ / AUXPLL_REF_DIV
//        * (AUXPLL_PRESCALER_DIV * AUXPLL_BCOUNTER + AUXPLL_ACOUNTER);

//    assert(AUXPLL_VCO_FREQ > 1450e6 && AUXPLL_VCO_FREQ < 1800e6); // datasheet
//    assert(AUXPLL_VCO_FREQ > 1499e6 && AUXPLL_VCO_FREQ < 1501e6); // 1500 MHz

//    assert(AUXPLL_PRESCALER >= 4 && AUXPLL_PRESCALER <= 6); // prescaler input > 1000 MHz
//    volatile const double AUXPLL_PRESC_OUT_FREQ = AUXPLL_VCO_FREQ / AUXPLL_PRESCALER_DIV;
//    assert(AUXPLL_PRESC_OUT_FREQ < 300e6); // datasheet

//    volatile const int AUXPLL_VCO_DIV = auxpll_vco_div_map[AUXPLL_VCO_DIV_INDEX];
//    volatile const double AUXPLL_DIST_FREQ = AUXPLL_VCO_FREQ / AUXPLL_VCO_DIV;
//    assert(AUXPLL_DIST_FREQ > 249.9e6 && AUXPLL_DIST_FREQ < 250.1e6); // 250 MHz

    BusInterface *bus = &d->dev.bus;

    // PLL normal operation (PLL on)
    AD9516_PFD_CP_REG_Type reg_pfd_cp;
    reg_pfd_cp.raw = 0x0;
    reg_pfd_cp.b.chargepump_current = 7; // 7: 4.8 mA
    reg_pfd_cp.b.chargepump_mode = 3; // 3: normal operation
    reg_pfd_cp.b.pll_powerdown = 0;
    ad9516_write1(bus, AD9516_REG1_PFD_CP, reg_pfd_cp.raw);

    // PLL settings
    // Select and enable a reference input
    // set R, N (P, A, B), PFD polarity, and I CP
    // according to the intended loop configuration
    uint16_t rdiv = AUXPLL_REF_DIV;
    ad9516_write1(bus, 0x011, rdiv & 0xFF);
    ad9516_write1(bus, 0x012, (rdiv >> 8) & 0x3F);

    uint16_t acounter = AUXPLL_ACOUNTER;
    ad9516_write1(bus, 0x013, acounter & 0x3F);

    uint16_t bcounter = AUXPLL_BCOUNTER;
    ad9516_write1(bus, 0x014, bcounter & 0xFF);
    ad9516_write1(bus, 0x015, (bcounter >> 8) & 0x3F);

    AD9516_Pll_Control pll_control;
    pll_control.pll_control_1.raw = 0x06;
    pll_control.pll_control_2.raw = 0x0;
    pll_control.pll_control_3.raw = 0x06;
    pll_control.pll_control_4.raw = 0x0;
    pll_control.pll_control_5.raw = 0x0;
    pll_control.pll_control_6.raw = 0x0;
    pll_control.pll_control_7.raw = 0x0;
    pll_control.pll_control_8.raw = 0x0;

    pll_control.pll_control_1.b.prescaler_p = AUXPLL_PRESCALER_INDEX;
    pll_control.pll_control_7.b.ref1_poweron = 1;

    ad9516_write1(bus, 0x016, pll_control.pll_control_1.raw);
    ad9516_write1(bus, 0x017, pll_control.pll_control_2.raw);
    ad9516_write1(bus, 0x018, pll_control.pll_control_3.raw);
    ad9516_write1(bus, 0x019, pll_control.pll_control_4.raw);
    ad9516_write1(bus, 0x01A, pll_control.pll_control_5.raw);
    ad9516_write1(bus, 0x01B, pll_control.pll_control_6.raw);
    ad9516_write1(bus, 0x01C, pll_control.pll_control_7.raw);
    ad9516_write1(bus, 0x01D, pll_control.pll_control_8.raw);

    // reset VCO calibration
    pll_control.pll_control_3.b.vco_cal_now = 0;
    ad9516_write1(bus, 0x018, pll_control.pll_control_3.raw);
    pllIoUpdate(d);

    // Use the VCO divider as source for the distribution section
    AD9516_VCO_Divider_REG_Type reg_vco_div;
    reg_vco_div.raw = 0;
    reg_vco_div.b.divider = AUXPLL_VCO_DIV_INDEX;
    ad9516_write1(bus, AD9516_REG1_VCO_DIVIDER, reg_vco_div.raw);

    // Select VCO as the source
    AD9516_Input_Clocks_REG_Type reg_clocks;
    reg_clocks.raw = 0;
    reg_clocks.b.bypass_vco_div = 0;
    reg_clocks.b.select_vco = 1;
    reg_clocks.b.powerdown_vco_clk = 0;
    reg_clocks.b.powerdown_vco_interface = 0;
    reg_clocks.b.powerdown_clock_inp = 0;
    ad9516_write1(bus, AD9516_REG1_CLOCKS, reg_clocks.raw);

    auxpll_output_setup(d);

    // initiate VCO calibration
    pll_control.pll_control_3.b.vco_cal_now = 1;
    ad9516_write1(bus, 0x018, pll_control.pll_control_3.raw);
    pllIoUpdate(d);

//    osDelay(100);
//    // reset VCO calibration
//    pll_control.pll_control_3.b.vco_cal_now = 0;
//    ad9516_write1(bus, 0x018, pll_control.pll_control_3.raw);
//    pllIoUpdate(d);

    //auxpllReadAllRegisters_unused(d);
    return true;
}

