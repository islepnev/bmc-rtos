/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "bsp_powermon_types.h"

bool pm_switches_isEqual(const pm_switches l, const pm_switches r)
{
    return l.switch_5v == r.switch_5v
           && l.switch_5v_fmc == r.switch_5v_fmc
           && l.switch_3v3 == r.switch_3v3
           && l.switch_2v5 == r.switch_2v5
           && l.switch_1v0_core == r.switch_1v0_core
           && l.switch_1v0_mgt == r.switch_1v0_mgt
           && l.switch_1v2_mgt == r.switch_1v2_mgt;
}
