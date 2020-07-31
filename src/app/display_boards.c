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
#include "log/logbuffer.h"
#include "log/logentry.h"
#include "devicelist_print.h"
#include "display.h"
#include "devices.h"
#include "powermon/dev_powermon_display.h"
#include "vxsiicm/dev_vxsiicm_types.h"

void display_boards(int y)
{
    print_goto(2, 1);
    printf("Boards\n" ANSI_CLEAR_EOL);
    printf(" # eeprom  exp  merr serr BMC  FPGA     up   all power therm  misc  fpga   pll" ANSI_CLEAR_EOL "\n");
    int line = 0;
    /*
    // FIXME
    const Dev_vxsiicm *vxsiicm= get_dev_vxsiicm();
    for (uint32_t i=0; i<VXSIIC_SLOTS; i++) {
        const vxsiic_slot_status_t *status = &vxsiicm->status.slot[i];
        const char *label = vxsiic_map_slot_to_label[i];
        if (0 == status->present)
            printf("%2s" ANSI_CLEAR_EOL "\n", label);
        else
            printf("%2s   %s    %s%s %4lu %4lu %2u.%-2u  %02lX %7lu  %s  %s  %s  %s  %s  %s" ANSI_CLEAR_EOL "\n",
                   label,
                   status->pp_state.eeprom_found ? " + ":" . ",
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
                   sensor_status_str(status->mcu_info.enc_status.b.misc),
                   sensor_status_str(status->mcu_info.enc_status.b.fpga),
                   sensor_status_str(status->mcu_info.enc_status.b.pll)
                   );
        line++;
    }
    print_clearbox(4+line, VXSIIC_SLOTS-line);
*/
}
