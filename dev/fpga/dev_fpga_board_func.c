/*
**    Copyright 2021 Ilia Slepnev
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

#include "dev_fpga_board_func.h"

#include <stdint.h>

#include "../ad9545/dev_ad9545.h"
#include "app_shared_data.h"
#include "dev_fpga_types.h"
#include "devicelist.h"
#include "fpga_io.h"
#include "log/log.h"

typedef union fpga_reg_clock_phase_detect_t {
    struct {
        uint16_t denom: 4;
        uint16_t numer: 4;
        uint16_t unused: 7;
        uint16_t time_valid: 1;
    } b;
    uint16_t raw;
} fpga_reg_clock_phase_detect_t;

enum {
    FPGA_REG_CLOCK_PHASE_CONTROL = 0x6000,
    FPGA_REG_CLOCK_PHASE_DETECT = 0x6001
};

bool write_clock_shift_status(Dev_fpga *dev)
{
    uint16_t ctrl = 0;
    if (pll_clock_shift_command.ack != pll_clock_shift_command.req) ctrl |= 1;
    if (! fpga_w16(dev, FPGA_REG_CLOCK_PHASE_CONTROL, ctrl))
        return false;
    return true;
}

bool fpgaBoardSpecificPoll(Dev_fpga *dev)
{
#if ENABLE_AD9545 && defined BOARD_ADC64VE
    write_clock_shift_status(dev);

    // check PLL lock status
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9545);
    if (!d || !d->priv)
        return false;
    const Dev_ad9545_priv *priv = (Dev_ad9545_priv *)device_priv_const(d);

    bool pll0_locked = (SENSOR_NORMAL == d->sensor) &&
                 priv->status.sysclk.b.pll0_locked;
    bool pll_ref_a_valid = priv->status.ref[0].b.valid;
    bool pll_ref_b_valid = priv->status.ref[2].b.valid;

    if (!pll0_locked|| !pll_ref_a_valid || !pll_ref_b_valid)
        return true;

    fpga_reg_clock_phase_detect_t data = {0};
    if (! fpga_r16(dev, FPGA_REG_CLOCK_PHASE_DETECT, &data.raw))
        return false;
    if (data.b.denom == 0)
        return true;
    if (data.b.numer == 0)
        return true;
    log_printf(LOG_INFO, "%s: %d/%d %s",
               dev->dev.name,
               data.b.numer,
               data.b.denom,
               data.b.time_valid?"time valid":"time invalid");
    if (pll_clock_shift_command.ack != pll_clock_shift_command.req) {
        log_printf(LOG_WARNING, "%s: clock shift FIFO busy", dev->dev.name);
        return true;
    }
    pll_clock_shift_command.numer = data.b.numer;
    pll_clock_shift_command.denom = data.b.denom;
    pll_clock_shift_command.req++;
    write_clock_shift_status(dev);
#endif
    return true;
}
