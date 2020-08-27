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

const AD9548_IRQMask_TypeDef AD9548_IRQMask_Default = {
    .v = {
        0x00,	// IRQ Pin Output Mode
        0x33,	// IRQ Mask for SYSCLK
        0x0F,	// IRQ Mask for Distribution Sync, Watchdog Timer, and EEPROM
        0xFF,	// IRQ Mask for the Digital PLL
        0x1F,	// IRQ Mask for History Update, Frequency Limit, and Phase Slew Limit
        0xFF,	// IRQ Mask for Reference Inputs (AA/A)
        0xFF,	// IRQ Mask for Reference Inputs (BB/B)
        0xFF,	// IRQ Mask for Reference Inputs (CC/C)
        0xFF	// IRQ Mask for Reference Inputs (DD/D)
    }
};
