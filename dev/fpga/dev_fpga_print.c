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

#include "dev_fpga_print.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "dev_fpga_types.h"
#include "devicelist.h"
#include "display.h"

static const char *fpga_state_str(fpga_state_t state)
{
    switch (state) {
    case FPGA_STATE_STANDBY: return ANSI_YELLOW "STANDBY" ANSI_CLEAR;
    case FPGA_STATE_RESET:   return ANSI_RED "RESET" ANSI_CLEAR;
    case FPGA_STATE_LOAD:    return ANSI_YELLOW "LOAD" ANSI_CLEAR;
    case FPGA_STATE_RUN:     return ANSI_GREEN "RUN" ANSI_CLEAR;
    case FPGA_STATE_PAUSE:   return ANSI_GREEN "IDLE" ANSI_CLEAR;
    case FPGA_STATE_ERROR:   return ANSI_RED "ERROR" ANSI_CLEAR;
    }
    return "unknown";
}

void dev_fpga_print_box(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);

    printf("FPGA %s",
           priv->initb ? "" : ANSI_RED "INIT_B low " ANSI_CLEAR);
    if (priv->initb && !priv->done)
        printf(ANSI_YELLOW "DONE low" ANSI_CLEAR);
    if (priv->done && priv->id_read) {
        uint64_t serial = (priv->ow_id >> 8) & 0xFFFFFFFFFFFF;
        int16_t rawTemp = priv->temp & 0xFFF;
        if (rawTemp & 0x800) rawTemp = -(rawTemp&0x7FF);
        double temp = rawTemp / 16.0;
        printf("%02X %08llX %.1f", priv->id, serial, temp);
    }
    printf(ANSI_CLEAR_EOL ANSI_COL30 "%9s ", fpga_state_str(priv->state));
    printf("%s", sensor_status_ansi_str(get_fpga_sensor_status()));
    printf("\n");
}
