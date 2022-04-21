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
#include "ad9548_print.h"
#include "ad9548_setup.h"
#include "ad9548_setup_regs.h"
#include "ad9548_status_regs.h"
#include "log/log.h"

typedef union {
    uint64_t value:48;
    uint8_t ch[6];
} reg48_t;

bool ad9548_poll_irq_state(BusInterface *bus, AD9548_Status *status)
{
    bool ok = true;
    bool irqFound = false;
    // Read IRQ status
    AD9548_IRQMask_TypeDef irq_flags;
    for (uint16_t i=0; i<8; i++) {
        if (! ad9548_read_register(bus, 0x0D02 + i, &irq_flags.v[i]))
            return false;
    }
    // Clear IRQ bits
    for (uint16_t i=0; i<8; i++) {
        if (irq_flags.v[i]) {
            irqFound = true;
            if (! ad9548_write_register(bus, 0x0A04 + i, irq_flags.v[i]))
                return false;
        }
    }
    if (irqFound) {
        if (! ad9548_ioupdate(bus))
            return false;
    }
    if (irq_flags.b.dpll_phase_locked) {
        // If locked on the input B, synchronize phase
        uint8_t data;
        if (! ad9548_read_register(bus, 0x0D0B, &data))
            return false;
        uint8_t active_ref = data & 0b00000111;
        if (active_ref == 0b010) {
            if (! ad9548_output_sync(bus))
                return false;
        }
    }
    if (irq_flags.b.dpll_phase_unlocked) {
        status->pll_unlock_cntr++;
    }
    ad9548_print_irq_status(&irq_flags); // debug only
    return ok;
}


bool ad9548_read_status(BusInterface *bus, AD9548_Status *status)
{
    bool ok = true;
    ok &= ad9548_ioupdate(bus);
    bool id_ok;
    ok &= ad9548_check_id(bus, &id_ok);
    ok &= ad9548_read_register(bus, 0x0D01, &status->sysclk.raw);
    if (status->sysclk.raw == 0xFF) {
        log_printf(LOG_ERR, "sysclk status: %02X", status->sysclk.raw);
        return false;
    }
    ok &= ad9548_read_register(bus, 0x0D0A, &status->DpllStat.raw);
    ok &= ad9548_read_register(bus, 0x0D0B, &status->DpllStat2.raw);
    ok &= ad9548_read_register(bus, 0x0500, &status->refPowerDown);
    for (uint16_t i=0; i<8; i++) {
        ok &= ad9548_read_register(bus, 0x0D0C + i, &status->refStatus[i].raw);
    }
    reg48_t reg48;
    for (int i=0; i<6; i++)
        ok &= ad9548_read_register(bus, 0x0D14+i, &reg48.ch[i]);
    status->holdover_ftw = reg48.value;
    return ok;
}

bool ad9548_read_sysclk_status(BusInterface *bus, AD9548_Status *status)
{
    return ad9548_read_register(bus, 0x0D01, &status->sysclk.raw);
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
