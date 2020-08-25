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

#include "ad9548_status.h"

#include "ad9548.h"
#include "ad9548_status_regs.h"

bool ad9548_read_status(BusInterface *bus, AD9548_Status *status)
{
    ad9548_ioupdate(bus);
    status->sysclk.raw = ad9548_read_register(bus, 0x0D01);
    status->DpllStat.raw =   ad9548_read_register(bus, 0x0D0A);
    status->refPowerDown = ad9548_read_register(bus, 0x0500);
    status->refActive = ad9548_read_register(bus, 0x0D0B) & 0x7;
    for (uint16_t i=0; i<8; i++) {
        status->refStatus[i] = (ad9548_read_register(bus, 0x0D0C + i) & 0x0F);
    }
    return true;
}

bool ad9548_read_sysclk_status(BusInterface *bus, AD9548_Status *status)
{
    status->sysclk.raw = ad9548_read_register(bus, 0x0D01);
    return true;
}

bool ad9548_isDpllLocked(AD9548_Status *status)
{
    return status->DpllStat.raw == 0x32;
}

bool ad9548_sysclk_is_locked(AD9548_Sysclk_Status_REG_Type sysclk)
{
    return sysclk.b.locked &&
           sysclk.b.stable &&
           !sysclk.b.cal_busy;
}
