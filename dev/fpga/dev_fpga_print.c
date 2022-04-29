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
#include <string.h>

#include "ansi_escape_codes.h"
#include "dev_fpga_types.h"
#include "devicelist.h"
#include "display.h"
#include "sdb_util.h"

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

char commit_id[16+1] = {0};
char version_str[16+1] = {0};
char product_name[19+1] = {0};

void dev_fpga_print_box(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    const Dev_fpga_gpio *gpio = &priv->gpio;
    const Dev_fpga_runtime *fpga = &priv->fpga;
    printf("FPGA %s",
           gpio->initb ? "" : ANSI_RED "INIT_B low " ANSI_CLEAR);
    if (gpio->initb && !gpio->done)
        printf(ANSI_YELLOW "DONE low" ANSI_CLEAR);
    bool sdb_ok = fpga->sdb.syn.date;
    if (gpio->done && (fpga->id_read || sdb_ok)) {
        uint64_t serial = (fpga->ow_id >> 8) & 0xFFFFFFFFFFFF;
        if (sdb_ok) {
            const Dev_fpga_sdb *sdb = &fpga->sdb;
            const struct sdb_synthesis *syn = &sdb->syn;
            sdb_copy_printable(commit_id, syn->commit_id, sizeof(syn->commit_id), '\0');
            if (!strlen(commit_id))
                snprintf(commit_id, sizeof (commit_id), "v%d.%d.%d",
                         (fpga->fw_ver >> 8) & 0xFF,
                         fpga->fw_ver & 0xFF,
                         fpga->fw_rev);
            const struct sdb_interconnect *ic = &sdb->ic;
            const struct sdb_product *product = &ic->sdb_component.product;
            snprint_sdb_version(version_str, sizeof(version_str), product->version);
            sdb_copy_printable(product_name, product->name, sizeof(product->name), '\0');
            if (!strlen(product_name))
                snprintf(product_name, sizeof(product_name), "%02X", fpga->id);
            printf("%04llX-%04llX  %s  %s",
                   serial >> 16, serial & 0xFFFF,
                   product_name,
                   commit_id);
        } else {
            printf("%02X  %04llX-%04llX  %d.%d.%d", fpga->id,
                   serial >> 16,
                   serial & 0xFFFF,
                   (fpga->fw_ver >> 8) & 0xFF,
                   fpga->fw_ver & 0xFF,
                   fpga->fw_rev);
        }
    }
    // printf(ANSI_CLEAR_EOL ANSI_COL50 "%9s ", fpga_state_str(priv->fsm.state));
    printf("%s", sensor_status_ansi_str(get_fpga_sensor_status()));
    printf("\n");
}
