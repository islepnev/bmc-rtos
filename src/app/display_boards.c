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

#include "display_log.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "devicelist_print.h"
#include "devicelist.h"
#include "display.h"
#include "log/log.h"
#include "powermon/dev_powermon_display.h"
#include "vxsiicm/dev_vxsiicm_types.h"

void display_boards_page(int y, bool repaint)
{
#ifdef BOARD_TTVXS
    print_goto(y, 1);
    printf("Boards" ANSI_CLEAR_EOL "\n");
    printf(" # exp  merr serr BMC  FPGA     up   all power therm  misc  fpga   pll" ANSI_CLEAR_EOL "\n");
    int cur_y = y + 2;
    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICM);
    if (d && d->priv) {
        const Dev_vxsiicm_priv *vxsiicm = (const Dev_vxsiicm_priv *)device_priv_const(d);

        int line = 0;
        for (uint32_t i=0; i<VXSIIC_SLOTS; i++) {
            const vxsiic_slot_status_t *status = &vxsiicm->status.slot[i];
            const char *label = vxsiic_map_slot_to_label[i];
            if (0 == status->present)
                printf("%2s" ANSI_CLEAR_EOL "\n", label);
            else
                printf("%2s  %s%s %4lu %4lu %2u.%-2u  %02lX %7lu  %s  %s  %s  %s  %s  %s" ANSI_CLEAR_EOL "\n",
                       label,
                       (status->ioexp & VXSIIC_PP_IOEXP_BIT_PGOOD) ? "P" : ".",
                       (status->ioexp & VXSIIC_PP_IOEXP_BIT_DONE) ? "D" : ".",
                       status->iic_master_stats.errors,
                       status->mcu_info.iic_stats.errors,
                       (uint16_t)(status->mcu_info.bmc_ver >> 16),
                       (uint16_t)status->mcu_info.bmc_ver,
                       status->mcu_info.module_id & 0xFF,
                       status->mcu_info.uptime,
                       sensor_status_str(status->mcu_info.enc_status.b.system),
                       sensor_status_str(status->mcu_info.enc_status.b.pm),
                       sensor_status_str(status->mcu_info.enc_status.b.therm),
                       sensor_status_str(status->mcu_info.enc_status.b.sfpiic),
                       sensor_status_str(status->mcu_info.enc_status.b.fpga),
                       sensor_status_str(status->mcu_info.enc_status.b.pll)
                       );
            line++;
        }
        cur_y += VXSIIC_SLOTS;
    }

    // print_clearbox(y+2+line, VXSIIC_SLOTS-line);
    if (repaint)
        print_clearbox(cur_y, screen_height - 1 - cur_y);
#endif
}
