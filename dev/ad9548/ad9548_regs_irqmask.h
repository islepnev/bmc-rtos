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

#ifndef AD9548_REGS_IRQMASK_H
#define AD9548_REGS_IRQMASK_H

#include <stdint.h>

enum { PLL_IRQMASK_SIZE = 9 };

#pragma pack(push, 1)

typedef union {
    uint8_t v[PLL_IRQMASK_SIZE];
} AD9548_IRQMask_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern const AD9548_IRQMask_TypeDef AD9548_IRQMask_Default;

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_IRQMASK_H
