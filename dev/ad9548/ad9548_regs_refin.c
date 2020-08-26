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

#include "ad9548_regs_refin.h"

const AD9548_RefIn_TypeDef AD9548_RefIn_Default = {
    .b.powerdown.raw = 0x00,
    .b.logic_family.raw = 0,
    .b.manual_select.b.a = 0,
    .b.manual_select.b.b = 1,
    .b.manual_select.b.c = 2,
    .b.manual_select.b.d = 3,
    .b.manual_select.b.en_a = 0,
    .b.manual_select.b.en_b = 0,
    .b.manual_select.b.en_c = 0,
    .b.manual_select.b.en_d = 0,
    .b.phase_buildout_switching.b.priority = 1
};
