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

const AD9548_MFPins_TypeDef AD9548_MFPins_Default = {
    .m_pin = {
        { .func = 64, .output = 1 },
        { .func = 66, .output = 1 },
        { .func = 68, .output = 1 },
        { .func = 70, .output = 1 },
        { .func =  7, .output = 1 },
        { .func = 11, .output = 1 },
        { .func = 21, .output = 1 },
        { .func = 22, .output = 1 }
    },
};
