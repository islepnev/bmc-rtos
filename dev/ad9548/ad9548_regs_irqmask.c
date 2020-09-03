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

#include "ad9548_regs_irqmask.h"

void AD9548_IRQPinMode_Default(AD9548_IRQPinMode_TypeDef *p)
{
    p->b.mode = 0;
}

void AD9548_IRQMask_Default(AD9548_IRQMask_TypeDef *p)
{
    for (int i=0; i<PLL_IRQMASK_SIZE; i++)
        p->v[i] = 0xFF; // enable all
    // ignore these interrupts
    p->b.hist_updated = 0;
    //    p->v[0] = 0x33;	// IRQ Mask for SYSCLK
    //    p->v[1] = 0x0F;	// IRQ Mask for Distribution Sync; Watchdog Timer; and EEPROM
    //    p->v[2] = 0xFF;	// IRQ Mask for the Digital PLL
    //    p->v[3] = 0x1F;	// IRQ Mask for History Update; Frequency Limit; and Phase Slew Limit
    //    p->v[4] = 0xFF;	// IRQ Mask for Reference Inputs (AA/A)
    //    p->v[5] = 0xFF;	// IRQ Mask for Reference Inputs (BB/B)
    //    p->v[6] = 0xFF;	// IRQ Mask for Reference Inputs (CC/C)
    //    p->v[7] = 0xFF;	// IRQ Mask for Reference Inputs (DD/D)
}
