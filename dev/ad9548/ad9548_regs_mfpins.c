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

#include "ad9548_regs_mfpins.h"

enum {
    AD9548_MF_PIN_REF_A_ACTIVE = 64,
    AD9548_MF_PIN_REF_B_ACTIVE = 66,
    AD9548_MF_PIN_REF_C_ACTIVE = 68,
    AD9548_MF_PIN_REF_D_ACTIVE = 70,
    AD9548_MF_PIN_SYSCLK_LOCK =  7,
    AD9548_MF_PIN_SYSCLK_STABLE = 11,
    AD9548_MF_PIN_DPLL_FREERUN = 16,
    AD9548_MF_PIN_DPLL_ACTIVE = 17,
    AD9548_MF_PIN_DPLL_HOLDOVER = 18,
    AD9548_MF_PIN_DPLL_SWITCHOVER = 19,
    AD9548_MF_PIN_DPLL_REF_PHASE_MASTER = 20,
    AD9548_MF_PIN_DPLL_PHASE_LOCK = 21,
    AD9548_MF_PIN_DPLL_FREQ_LOCK = 22,
};

void AD9548_MFPins_Default(AD9548_MFPins_TypeDef *p)
{
    for (int i=0; i<PLL_MFPINS_SIZE; i++)
        p->m_pin[i].output = 1;

    // D1 orange
    p->m_pin[0].func = AD9548_MF_PIN_REF_A_ACTIVE;

    // D2 green
    p->m_pin[1].func = AD9548_MF_PIN_REF_B_ACTIVE;

    // D3 orange
    p->m_pin[2].func = AD9548_MF_PIN_REF_D_ACTIVE;

    // D4 green
    p->m_pin[3].func = AD9548_MF_PIN_REF_C_ACTIVE;

    // D5 orange
    p->m_pin[4].func = AD9548_MF_PIN_DPLL_HOLDOVER;

    // D6 green
    p->m_pin[5].func = AD9548_MF_PIN_DPLL_PHASE_LOCK;

    // D7 orange
    p->m_pin[6].func = AD9548_MF_PIN_DPLL_FREERUN;

    // D8 green
    p->m_pin[7].func = AD9548_MF_PIN_DPLL_REF_PHASE_MASTER;
}
