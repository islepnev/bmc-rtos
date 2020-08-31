/*
**    Copyright 2019 Ilja Slepnev
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

#ifndef AD9548_LOOP_FILTER_DESIGN_H
#define AD9548_LOOP_FILTER_DESIGN_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ad9548_regs_profile.h"

void ad9548_profile_set_iir(AD9548_Profile_TypeDef *p, const double coef[4]);
bool ad9548_design_iir(double fs, double D, double fp, double theta,
                       double foffset, double atten, double coef[4]);

#ifdef __cplusplus
}
#endif

#endif // AD9548_LOOP_FILTER_DESIGN_H
