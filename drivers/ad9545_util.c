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

#include "ad9545_util.h"
#include <stdio.h>
#include "ad9545_setup.h"

int64_t pll_ftw_rel_ppb(const Dev_ad9545 *d, PllChannel_TypeDef channel)
{
    const DPLL_Status *dpll_status = &d->status.dpll[channel];
    uint64_t ftw = dpll_status->ftw_history;
    uint64_t default_ftw = get_dpll_default_ftw(channel);
    int64_t twdelta = ftw - default_ftw;
    int64_t norm = default_ftw/1000000000ULL;
    return twdelta/norm;
}

void pllPrintRefStatusBits(Ref_Status_REG_Type r)
{
    printf("%s%s%s%s%s%s",
           r.b.slow   ? " SLOW" : "",
           r.b.fast   ? " FAST" : "",
           r.b.jitter ? " JITTER" : "",
           r.b.fault  ? " FAULT" : "",
           r.b.valid  ? " VALID" : "",
           r.b.los    ? " LOS" : ""
                        );
}

const char *pllProfileRefSourceStr(ProfileRefSource_TypeDef r)
{
    switch(r) {
    case PROFILE_REF_SOURCE_A: return "A";
    case PROFILE_REF_SOURCE_AA: return "AA";
    case PROFILE_REF_SOURCE_B: return "B";
    case PROFILE_REF_SOURCE_BB: return "BB";
    case PROFILE_REF_SOURCE_DPLL0: return "DPLL0";
    case PROFILE_REF_SOURCE_DPLL1: return "DPLL1";
    case PROFILE_REF_SOURCE_NCO0: return "NCO0";
    case PROFILE_REF_SOURCE_NCO1: return "NCO1";
    case PROFILE_REF_SOURCE_INVALID: return "";
    default: return "";
    }
}

ProfileRefSource_TypeDef pll_get_current_ref(const Dev_ad9545 *d, PllChannel_TypeDef channel)
{
//    int active = d->status.dpll[channel].operation.b.active;
//    if (!active)
//        return PROFILE_REF_SOURCE_INVALID;
    uint8_t active_profile = d->status.dpll[channel].operation.b.active_profile;
    if (active_profile >= DPLL_PROFILE_COUNT)
        return PROFILE_REF_SOURCE_INVALID;
    Pll_DPLL_Setup_TypeDef dpll = {0};
    init_DPLL_Setup(&dpll, channel);
    return dpll.profile[active_profile].Profile_Ref_Source;
}
