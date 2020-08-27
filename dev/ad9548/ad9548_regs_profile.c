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

#include "ad9548_regs_profile.h"

const AD9548_Profile_TypeDef *PLL_Prof_default(AD9548_Profile_TypeDef *p)
{
    p->b.phase_lock_scale=0;
    p->b.V=0;
    p->b.U=0;
    p->b.validation_timer=100;
    p->b.redetect_timeout=100;
    p->b.phase_lock_threshold=100;
    p->b.phase_lock_fill_rate=32;
    p->b.phase_lock_drain_rate=68;
    p->b.freq_lock_threshold=100;
    p->b.freq_lock_fill_rate=32;
    p->b.freq_lock_drain_rate=68;
    return p;
}
