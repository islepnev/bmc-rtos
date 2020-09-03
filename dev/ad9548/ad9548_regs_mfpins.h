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

#ifndef AD9548_REGS_MFPINS_H
#define AD9548_REGS_MFPINS_H

#include <stdint.h>

enum { PLL_MFPINS_SIZE = 8 };

#pragma pack(push, 1)
typedef struct {
    uint8_t func:7;
    uint8_t output:1;
} AD9548_MFunction_TypeDef;

typedef union {
    AD9548_MFunction_TypeDef m_pin[PLL_MFPINS_SIZE];
    uint8_t v[PLL_MFPINS_SIZE];
} AD9548_MFPins_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

void AD9548_MFPins_Default(AD9548_MFPins_TypeDef *p);

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_MFPINS_H
