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

#include "ad9548_setup.h"

#include <string.h>

#include "ad9548.h"
#include "ad9548_setup_regs.h"
#include "ad9548_status_regs.h"
#include "ad9548_regs_profile_tdc_vhle.h"
#include "log/log.h"

uint16_t pll_unlock_cntr;

void ad9548_output_sync(BusInterface *bus)
{
    ad9548_write_register(bus, 0x0A02, 0x02);
    ad9548_ioupdate(bus);
    ad9548_write_register(bus, 0x0A02, 0x00);
    ad9548_ioupdate(bus);
}

void ad9548_poll_irq_state(BusInterface *bus)
{
    bool irqFound = false;
    // Read IRQ status
    uint8_t irq_flags[8];
    for (uint16_t i=0; i<8; i++) {
        irq_flags[i] = ad9548_read_register(bus, 0x0D02 + i);
    }
    // Clear IRQ bits
    for (uint16_t i=0; i<8; i++) {
        if (irq_flags[i]) {
            irqFound = true;
            ad9548_write_register(bus, 0x0A04 + i, irq_flags[i]);
        }
    }
    if (irqFound)
        ad9548_ioupdate(bus);
    if (irq_flags[2] & 1) {
        // If locked on the input B, synchronize phase
        if ((ad9548_read_register(bus, 0x0D0B) & 0b00000111) == 0b010)
        {
            ad9548_output_sync(bus);
        }
    }
    if (irq_flags[2] & 2) {
        pll_unlock_cntr++;
    }
}

bool ad9548_software_reset(BusInterface *bus)
{
    AD9548_Control_REG1_Type ctrl;
    ctrl.raw = 0;
    ctrl.b.long_instr = 1;
    ctrl.b.sdo_enable = 1;
    ctrl.b.softreset = 1;
    bool ok = true;
    ok &= ad9548_write_register(bus, 0x0000, ctrl.raw);

    ctrl.b.softreset = 0;
    ok &= ad9548_write_register(bus, 0x0000, ctrl.raw);
    return ok;
}

bool ad9548_setup_sysclk(BusInterface *bus, ad9548_setup_t *reg)
{
    for (unsigned int i = 0; i < PLL_SYSCLK_SIZE; i++)
    {
        //ad9548_write_register(bus, reg->PLL_SysClk[i].address, reg->PLL_SysClk[i].data);
        ad9548_write_register(bus, AD9545_REG_SYSCLK_BASE+i, reg->sysclk.v[i]);
    }

    ad9548_ioupdate(bus);
    return true;
}

bool ad9548_calibrate_sysclk(BusInterface *bus, ad9548_setup_t *reg)
{
    ad9548_write_register(bus, 0x0A02, 0x01);	// Initiate the system clock calibration process
    ad9548_ioupdate(bus);
    ad9548_write_register(bus, 0x0A02, 0x00);	// Clear the initiation bit
    ad9548_ioupdate(bus);
    return true;
}

bool ad9548_initial_setup(BusInterface *bus, ad9548_setup_t *reg)
{
    //	ad9548_write_register(bus, 0x0000, 0x10); // Enable 4-wire SPI
    ad9548_setup_sysclk(bus, reg);
    ad9548_calibrate_sysclk(bus, reg);
    bool sysclk_ok = false;
    for(int i=0; i<1000; i++) {
        AD9548_Sysclk_Status_REG_Type sysclk;
        sysclk.raw = ad9548_read_register(bus, 0x0D01);
        sysclk_ok = sysclk.b.locked && !sysclk.b.cal_busy && sysclk.b.stable;
        if (sysclk_ok) {
            break;
        }
    }
    if (!sysclk_ok) {
        return false;
    }
    for (unsigned int i = 0; i < PLL_MFPINS_SIZE; i++)
    {
        ad9548_write_register(bus, reg->PLL_MFPins[i].address, reg->PLL_MFPins[i].data);
    }

    for (unsigned int i = 0; i < PLL_IRQ_SIZE; i++)
    {
        ad9548_write_register(bus, reg->PLL_IRQ[i].address, reg->PLL_IRQ[i].data);
    }

    for (unsigned int i = 0; i < PLL_DPLL_SIZE; i++)
    {
        ad9548_write_register(bus, AD9545_REG_DPLL_BASE+i, reg->dpll.v[i]);
    }

    ad9548_ioupdate(bus);

    ad9548_poll_irq_state(bus); // clear pending IRQs

    return sysclk_ok;
}

bool ad9548_ProfileConfig(BusInterface *bus, ad9548_setup_t *reg)
{
    for (unsigned int i = 0; i < PLL_OUTCLK_SIZE; i++)
    {
        ad9548_write_register(bus, reg->PLL_OutClk[i].address, reg->PLL_OutClk[i].data);
    }

    for (unsigned int i = 0; i < PLL_REFIN_SIZE; i++)
    {
        ad9548_write_register(bus, AD9545_REG_REFIN_BASE+i, reg->refin.v[i]);
    }

    for (int i=0; i < PLL_PROF_SIZE; i++)
        ad9548_write_register(bus, AD9545_REG_PROFILE_0_BASE+i, reg->prof[0].v[i]);
    for (int i=0; i < PLL_PROF_SIZE; i++)
        ad9548_write_register(bus, AD9545_REG_PROFILE_1_BASE+i, reg->prof[1].v[i]);
    for (int i=0; i < PLL_PROF_SIZE; i++)
        ad9548_write_register(bus, AD9545_REG_PROFILE_2_BASE+i, reg->prof[2].v[i]);
    for (int i=0; i < PLL_PROF_SIZE; i++)
        ad9548_write_register(bus, AD9545_REG_PROFILE_3_BASE+i, reg->prof[3].v[i]);

    ad9548_write_register(bus, 0x0A0D, 0xFF); // Start profile selection FSM

    ad9548_ioupdate(bus);
    return true;
}

bool ad9548_setup(BusInterface *bus, ad9548_setup_t *setup)
{
    ad9548_initial_setup(bus, setup);
    ad9548_ProfileConfig(bus, setup);
    return true;
}

void ad9548_Phase_Shift_Right(BusInterface *bus)
{
    ad9548_write_register(bus, 0x0A0C, 0b00000001);
}

void ad9548_Phase_Shift_Left(BusInterface *bus)
{
    ad9548_write_register(bus, 0x0A0C, 0b00000010);
}

void ad9548_Phase_Reset(BusInterface *bus)
{
    ad9548_write_register(bus, 0x0A0C, 0b00000100);
}


#define COPY_ARRAY(dest, src) \
    do {\
        memcpy(dest, src, sizeof(dest)); \
    } while (0);

void ad9548_setProfile(ad9548_setup_t *reg, AD9548_BOARD_PLL_VARIANT variant)
{
    memcpy(reg->sysclk.v, AD9548_Sysclk_Default.v, PLL_SYSCLK_SIZE);
    memcpy(reg->PLL_MFPins, Default_PLL_MFPins, sizeof(Default_PLL_MFPins));
    memcpy(reg->PLL_IRQ, Default_PLL_IRQ, sizeof(Default_PLL_IRQ));
    memcpy(reg->dpll.v, AD9548_Dpll_Default.v, PLL_DPLL_SIZE);
    memcpy(reg->refin.v, AD9548_RefIn_Default.v, PLL_REFIN_SIZE);

    switch (variant)
    {
    case BOARD_PLL_DEFAULT:
        // __attribute__ ((fallthrough));
    case BOARD_PLL_ADC64VE:
        COPY_ARRAY(reg->PLL_OutClk, PLL_OutClk_ADC64VE);
//        COPY_ARRAY(reg->PLL_RefIn, &AD9548_RefIn_Default);
//        memcpy(reg->prof[0].v, PLL_Prof0_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[1].v, PLL_Prof1_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[2].v, PLL_Prof2_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[3].v, PLL_Prof3_TDC_VHLE.v, PLL_PROF_SIZE);
        break;
    case BOARD_PLL_TDC_VHLE:
        COPY_ARRAY(reg->PLL_OutClk, PLL_OutClk_TDC_VHLE);
//        COPY_ARRAY(reg->PLL_RefIn, &AD9548_RefIn_Default);
        memcpy(reg->prof[0].v, PLL_Prof0_TDC_VHLE.v, PLL_PROF_SIZE);
        memcpy(reg->prof[1].v, PLL_Prof1_TDC_VHLE.v, PLL_PROF_SIZE);
        memcpy(reg->prof[2].v, PLL_Prof2_TDC_VHLE.v, PLL_PROF_SIZE);
        memcpy(reg->prof[3].v, PLL_Prof3_TDC_VHLE.v, PLL_PROF_SIZE);
        break;
    case BOARD_PLL_TQDC16VS:
        COPY_ARRAY(reg->PLL_OutClk, PLL_OutClk_TQDC16VS);
//        COPY_ARRAY(reg->PLL_RefIn, &AD9548_RefIn_Default);
//        memcpy(reg->prof[0].v, PLL_Prof0_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[1].v, PLL_Prof1_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[2].v, PLL_Prof2_TDC_VHLE.v, PLL_PROF_SIZE);
//        memcpy(reg->prof[3].v, PLL_Prof3_TDC_VHLE.v, PLL_PROF_SIZE);
        break;
    }
}

void init_ad9548_setup(ad9548_setup_t *setup)
{
    ad9548_setProfile(setup, BOARD_PLL_TDC_VHLE); // FIXME
}
