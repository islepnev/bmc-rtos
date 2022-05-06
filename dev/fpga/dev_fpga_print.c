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
#include "crc8_dallas_maxim.h"

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

static bool onewire_crc_valid(uint64_t ow_id)
{
    const uint8_t ow_crc = ow_id >> 56;
    unsigned char crc = 0;
    for (int i=0; i<7; i++)
        crc = crc8_dallas_maxim((ow_id >> i*8) & 0xFF, crc);
    return crc == ow_crc;
}

bool onewire_id_valid(uint64_t ow_id)
{
    return ((ow_id & 0xFF) == 0x28) && onewire_crc_valid(ow_id);
}

char fpga_sdb_commit_id[16+1] = {0};
//char fpga_version_str[16+1] = {0};
char fpga_product_name[19+1] = {0};
char fpga_ow_serial_str[14+1] = {0};

void decode_fpga_info(const Dev_fpga_priv *priv)
{
    const Dev_fpga_runtime *fpga = &priv->fpga;
    bool ow_ok = onewire_id_valid(fpga->csr.ow_id);
    if (ow_ok) {
        uint64_t serial = (fpga->csr.ow_id >> 8) & 0xFFFFFFFFFFFF;
        snprintf(fpga_ow_serial_str, sizeof(fpga_ow_serial_str), "%04llX-%04llX",
                 serial >> 16, serial & 0xFFFF);
    } else {
        fpga_ow_serial_str[0] = '\0';
    }

    const Dev_fpga_sdb *sdb = &fpga->sdb;
    const struct sdb_synthesis *syn = &sdb->syn;
    sdb_copy_printable(fpga_sdb_commit_id, syn->commit_id, sizeof(syn->commit_id), '\0');
    if (!strlen(fpga_sdb_commit_id) && (fpga->csr.fw_ver != 0 || fpga->csr.fw_rev != 0))
        snprintf(fpga_sdb_commit_id, sizeof (fpga_sdb_commit_id), "v%d.%d.%d",
                 (fpga->csr.fw_ver >> 8) & 0xFF,
                 fpga->csr.fw_ver & 0xFF,
                 fpga->csr.fw_rev);
    const struct sdb_interconnect *ic = &sdb->ic;
    const struct sdb_product *product = &ic->sdb_component.product;
//    snprint_sdb_version(fpga_version_str, sizeof(fpga_version_str), product->version);
    sdb_copy_printable(fpga_product_name, product->name, sizeof(product->name), '\0');
    if (!strlen(fpga_product_name))
        snprintf(fpga_product_name, sizeof(fpga_product_name), "%02X", fpga->csr.id);
}

void dev_fpga_print_comm_state(const Dev_fpga_priv *priv)
{
    const Dev_fpga_gpio *gpio = &priv->gpio;
    const Dev_fpga_runtime *fpga = &priv->fpga;
    const Dev_fpga_sdb *sdb = &fpga->sdb;
    bool sdb_ok = (sdb->ic.sdb_magic == SDB_MAGIC);
    printf("FPGA");
    if (!gpio->initb)
        printf(ANSI_RED " INIT_B low" ANSI_CLEAR);
    if (gpio->initb && !gpio->done)
        printf(ANSI_YELLOW " DONE low" ANSI_CLEAR);
    if (gpio->initb && gpio->done) {
        if (fpga->proto_version == 0) {
            printf(ANSI_RED " SPI: no connection" ANSI_CLEAR);
        } else {
            printf(" SPI v%d", fpga->proto_version);
            if (!fpga->csr_read) {
                printf(ANSI_RED " No CSR" ANSI_CLEAR);
            } else {
            }
            if (!sdb_ok)
                printf(ANSI_RED " No SDB" ANSI_CLEAR);
        }
    }
    if (gpio->done && fpga->csr_read) {
        decode_fpga_info(priv);
        printf("  %s  %s  %s", fpga_ow_serial_str, fpga_product_name, fpga_sdb_commit_id);
    }
}

void dev_fpga_print_box(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    const Dev_fpga_gpio *gpio = &priv->gpio;
    const Dev_fpga_runtime *fpga = &priv->fpga;
    dev_fpga_print_comm_state(priv);
    // printf(ANSI_CLEAR_EOL ANSI_COL50 "%9s ", fpga_state_str(priv->fsm.state));
    printf("%s", sensor_status_ansi_str(get_fpga_sensor_status()));
    printf("\n");
}
