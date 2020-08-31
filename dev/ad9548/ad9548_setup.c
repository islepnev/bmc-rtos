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
#include "board_config_ad9548.h"
#include "cmsis_os.h"
#include "log/log.h"

uint16_t pll_unlock_cntr;

bool ad9548_output_sync(BusInterface *bus)
{
    return
        ad9548_write_register(bus, 0x0A02, 0x02) &&
        ad9548_ioupdate(bus) &&
        ad9548_write_register(bus, 0x0A02, 0x00) &&
        ad9548_ioupdate(bus);
}

bool ad9548_poll_irq_state(BusInterface *bus)
{
    bool ok = true;
    bool irqFound = false;
    // Read IRQ status
    uint8_t irq_flags[8];
    for (uint16_t i=0; i<8; i++) {
        ok &= ad9548_read_register(bus, 0x0D02 + i, &irq_flags[i]);
    }
    // Clear IRQ bits
    for (uint16_t i=0; i<8; i++) {
        if (irq_flags[i]) {
            irqFound = true;
            ok &= ad9548_write_register(bus, 0x0A04 + i, irq_flags[i]);
        }
    }
    if (irqFound)
        ok &= ad9548_ioupdate(bus);
    if (irq_flags[2] & 1) {
        // If locked on the input B, synchronize phase
        uint8_t data;
        ok &= ad9548_read_register(bus, 0x0D0B, &data);
        if ((data & 0b00000111) == 0b010) {
            ok &= ad9548_output_sync(bus);
        }
    }
    if (irq_flags[2] & 2) {
        pll_unlock_cntr++;
    }
    return ok;
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
    bool ok = true;
    for (unsigned int i = 0; i < PLL_SYSCLK_SIZE; i++)
    {
        // ok &= ad9548_write_register(bus, reg->PLL_SysClk[i].address, reg->PLL_SysClk[i].data);
        ok &= ad9548_write_register(bus, AD9545_REG_SYSCLK_BASE+i, reg->sysclk.v[i]);
    }

    ok &= ad9548_ioupdate(bus);
    return ok;
}

bool ad9548_calibrate_sysclk(BusInterface *bus, ad9548_setup_t *reg)
{
    (void) reg;
    return
        ad9548_write_register(bus, 0x0A02, 0x01) &&	// Initiate the system clock calibration process
        ad9548_ioupdate(bus) &&
        ad9548_write_register(bus, 0x0A02, 0x00) &&	// Clear the initiation bit
        ad9548_ioupdate(bus);
}

bool ad9548_initial_setup(BusInterface *bus, ad9548_setup_t *reg)
{
    //	ad9548_write_register(bus, 0x0000, 0x10); // Enable 4-wire SPI
    if (!ad9548_setup_sysclk(bus, reg))
        return false;
    if (!ad9548_calibrate_sysclk(bus, reg))
        return false;

    bool sysclk_ok = false;
    for(int i=0; i<100; i++) {
        AD9548_Sysclk_Status_REG_Type sysclk;
        if (!ad9548_read_register(bus, 0x0D01, &sysclk.raw)) {
            sysclk.raw = 0;
            return false;
        }
        sysclk_ok = sysclk.b.locked && !sysclk.b.cal_busy && sysclk.b.stable;
        if (sysclk_ok)
            break;
        osDelay(1);
    }
    if (!sysclk_ok)
        return false;

    for (unsigned int i = 0; i < PLL_MFPINS_SIZE; i++)
    {
        if (!ad9548_write_register(bus, AD9545_REG_GENERAL_CONFIG_BASE+i, reg->mfpins.v[i]))
            return false;
    }

    for (unsigned int i = 0; i < PLL_IRQMASK_SIZE; i++)
    {
        if (!ad9548_write_register(bus, AD9545_REG_GENERAL_CONFIG_BASE+8+i, reg->irqmask.v[i]))
            return false;
    }

    for (unsigned int i = 0; i < PLL_DPLL_SIZE; i++)
    {
        if (!ad9548_write_register(bus, AD9545_REG_DPLL_BASE+i, reg->dpll.v[i]))
            return false;
    }

    if (!ad9548_ioupdate(bus))
        return false;

    if (!ad9548_poll_irq_state(bus))
        return false; // clear pending IRQs

    return true;
}

bool ad9548_ProfileConfig(BusInterface *bus, ad9548_setup_t *reg)
{
    for (unsigned int i = 0; i < PLL_OUTCLK_SIZE; i++)
    {
        if (!ad9548_write_register(bus, AD9545_REG_OUTPUT_BASE, reg->output.v[i]))
            return false;
    }

    for (unsigned int i = 0; i < PLL_REFIN_SIZE; i++)
    {
        if (!ad9548_write_register(bus, AD9545_REG_REFIN_BASE+i, reg->refin.v[i]))
            return false;
    }

    int base[AD9548_DPLL_PROFILE_COUNT] = {
        AD9545_REG_PROFILE_0_BASE,
        AD9545_REG_PROFILE_1_BASE,
        AD9545_REG_PROFILE_2_BASE,
        AD9545_REG_PROFILE_3_BASE,
        AD9545_REG_PROFILE_4_BASE,
        AD9545_REG_PROFILE_5_BASE,
        AD9545_REG_PROFILE_6_BASE,
        AD9545_REG_PROFILE_7_BASE
    };
    for (int b=0; b<AD9548_DPLL_PROFILE_COUNT; b++) {
        for (int i=0; i < PLL_PROF_SIZE; i++)
            if (!ad9548_write_register(bus, base[b]+i, reg->prof[b].v[i]))
                return false;
    }

    if (!ad9548_write_register(bus, 0x0A0D, 0xFF))
        return false; // Start profile selection FSM

    if (!ad9548_ioupdate(bus))
        return false;
    return true;
}

bool ad9548_setup(BusInterface *bus, ad9548_setup_t *setup)
{
    return
        ad9548_initial_setup(bus, setup) &&
        ad9548_ProfileConfig(bus, setup);
}

bool ad9548_Phase_Shift_Right(BusInterface *bus)
{
    return ad9548_write_register(bus, 0x0A0C, 0b00000001);
}

bool ad9548_Phase_Shift_Left(BusInterface *bus)
{
    return ad9548_write_register(bus, 0x0A0C, 0b00000010);
}

bool ad9548_Phase_Reset(BusInterface *bus)
{
    return ad9548_write_register(bus, 0x0A0C, 0b00000100);
}

void ad9548_setProfile(ad9548_setup_t *reg, AD9548_BOARD_PLL_VARIANT variant)
{
    memcpy(reg->sysclk.v, AD9548_Sysclk_Default.v, PLL_SYSCLK_SIZE);
    memcpy(reg->mfpins.v, AD9548_MFPins_Default.v, PLL_MFPINS_SIZE);
    memcpy(reg->irqmask.v, AD9548_IRQMask_Default.v, PLL_IRQMASK_SIZE);
    memcpy(reg->dpll.v, AD9548_Dpll_Default.v, PLL_DPLL_SIZE);
    memcpy(reg->refin.v, AD9548_RefIn_Default.v, PLL_REFIN_SIZE);
    for (int i=0; i<AD9548_DPLL_PROFILE_COUNT; i++)
        PLL_Prof_default(&reg->prof[i]);

    switch (variant)
    {
    case BOARD_PLL_DEFAULT:
        // __attribute__ ((fallthrough));
    case BOARD_PLL_ADC64VE:
        memcpy(reg->output.v, PLL_Output_ADC64VE.v, PLL_OUTCLK_SIZE);
        PLL_Prof0_ADC64VE(&reg->prof[0]);
        PLL_Prof1_ADC64VE(&reg->prof[1]);
        PLL_Prof2_ADC64VE(&reg->prof[2]);
        PLL_Prof3_ADC64VE(&reg->prof[3]);
        break;
    case BOARD_PLL_TDC_VHLE:
        memcpy(reg->output.v, PLL_Output_TDC_VHLE.v, PLL_OUTCLK_SIZE);
        PLL_Prof7_TDC_VHLE(&reg->prof[7]);
        PLL_Prof6_TDC_VHLE(&reg->prof[6]);
        PLL_Prof5_TDC_VHLE(&reg->prof[5]);
        PLL_Prof1_TDC_VHLE(&reg->prof[1]);
        PLL_Prof4_TDC_VHLE(&reg->prof[4]);
        break;
    case BOARD_PLL_TQDC16VS:
        memcpy(reg->output.v, PLL_Output_TQDC16VS.v, PLL_OUTCLK_SIZE);
        PLL_Prof0_TQDC16VS(&reg->prof[0]);
        PLL_Prof1_TQDC16VS(&reg->prof[1]);
        PLL_Prof2_TQDC16VS(&reg->prof[2]);
        PLL_Prof3_TQDC16VS(&reg->prof[3]);
        break;
    }
}

void init_ad9548_setup(ad9548_setup_t *setup)
{
#if defined (BOARD_ADC64VE)
    ad9548_setProfile(setup, BOARD_PLL_ADC64VE);
#elif defined (BOARD_TDC72VHLV2) || defined (BOARD_TDC72VHLV3)
    ad9548_setProfile(setup, BOARD_PLL_TDC_VHLE);
#elif defined (BOARD_TQDC16VS)
    ad9548_setProfile(setup, BOARD_PLL_TQDC16VS);
#endif
}
