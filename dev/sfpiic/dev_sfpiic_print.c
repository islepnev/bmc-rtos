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

#include "dev_sfpiic_print.h"

#include <stdio.h>
#include <string.h>

#include "dev_sfpiic.h"
#include "dev_sfpiic_types.h"
#include "display.h"
#include "sff/sff-common.h"
#include "sff/sff-internal.h"
#include "cmsis_os.h"
#include "ansi_escape_codes.h"

void sfpPrintSummaryHeader(void)
{
    printf("%-4s %-13s  %-16s  %-16s  %-5s  %-6s  %6s  %6s\n",
           "Port",
           "Type",
           "Vendor_name",
           "Vendor_serial",
           "Temp",
           "Volt.",
           "Rx dBm",
           "Tx dBm"
           );
}

const char *ethernet_compliance_str(const u8 id[256])
{
    // BUG: returns only first match
    /* 10G Ethernet Compliance Codes */
    if (id[3] & (1 << 7))
        return "10G Base-ER";
    if (id[3] & (1 << 6))
        return "10G Base-LRM";
    if (id[3] & (1 << 5))
        return "10G Base-LR";
    if (id[3] & (1 << 4))
        return "10G Base-SR";
    /* Ethernet Compliance Codes */
    if (id[6] & (1 << 7))
        return "BASE-PX";
    if (id[6] & (1 << 6))
        return "BASE-BX10";
    if (id[6] & (1 << 5))
        return "100BASE-FX";
    if (id[6] & (1 << 4))
        return "100BASE-LX/LX10";
    if (id[6] & (1 << 3))
        return "1000BASE-T";
    if (id[6] & (1 << 2))
        return "1000BASE-CX";
    if (id[6] & (1 << 1))
        return "1000BASE-LX";
    if (id[6] & (1 << 0))
        return "1000BASE-SX";
    if (id[7] || id[8] || id[9] || id[10])
        return "Fibre Channel";
    return "unsupported";
}

void sfpPrintSummaryStatus(const Dev_sfpiic_priv *d)
{
    const uint32_t now = osKernelSysTick();
    sfpPrintSummaryHeader();
    for(int row=0; row<SFPIIC_CH_CNT; ++row) {
        int sfp=0;
        if(row<3)
            sfp = 2-row;
        else
            sfp = SFPIIC_CH_CNT-row+2;

        const sfpiic_ch_status_t *status = &d->status.sfp[sfp];
        printf(" %-3d ", sfp+1);
        if (!status->present) {
            printf("\n");
            continue;
        }
        switch (status->ch_state) {
        case SFPIIC_CH_STATE_READY: {
            bool id_updated = (uint32_t)(now - status->id_updated_timetick) < 3000;
            bool dom_updated = (uint32_t)(now - status->dom_updated_timetick) < 3000;
            printf("%s%-13s  %-16s  %-16s",
                   id_updated ? "" : ANSI_RED,
                   ethernet_compliance_str(status->idprom),
                   status->vendor_name, status->vendor_serial
                   );
            if (status->dom_supported) {
                printf("%s  %-5.1f  %-6.2f",
                       dom_updated ? ANSI_CLEAR : ANSI_RED,
                       status->temp, status->voltage
                       );
                if (!status->multilane) {
                    printf("  %-6.1f  %-6.1f",
                           status->rx_power_dBm[0], status->tx_power_dBm[0]
                           );

                }
            }
            printf(ANSI_CLEAR "\n");
            break;
        }
        case SFPIIC_CH_STATE_UNKNOWN:
            printf(" STATE_UNKNOWN\n");
            break;
        case SFPIIC_CH_STATE_ERROR:
            printf(" STATE_ERROR\n");
            break;
        }
    }
}

void sfpPrintChannelHeader(void)
{
    printf("\n"
           "Port ch "
           "Rx_dBm Tx_dBm"
           "\n");
}

static void sfpPrintChannelStatus(const Dev_sfpiic_priv *d)
{
    int print_header = 1;
    for(int row=3; row<SFPIIC_CH_CNT; ++row) {
        int sfp = SFPIIC_CH_CNT-row+2;
        const sfpiic_ch_status_t *status = &d->status.sfp[sfp];
        if(!status->present)
            continue;
        if (!status->multilane)
            continue;
        if(print_header) {
            sfpPrintChannelHeader();
            print_header=0;
        } else {
            printf("--------------\n");

        }
        for(int ch=0; ch<4; ++ch) {
            printf(" %-4d %-3d"
                   "%-6.2f %-6.2f"
                   "\n"
                   , sfp+1, ch+1
                   , status->rx_power_dBm[ch], status->tx_power_dBm[ch]
                   );
        }
    }
}

void sfpPrintStatus(void)
{    
    const Dev_sfpiic_priv *priv = get_sfpiic_priv_const();
    if (!priv)
        return;
    printf("SFP status:\n");
//    sff8079_show_all(priv->status.sfp[0].idprom);
//    sff8472_show_all(priv->status.sfp[0].diag);

    sfpPrintSummaryStatus(priv);
    sfpPrintChannelStatus(priv);
}

void dev_sfpiic_print(void)
{
    const Dev_sfpiic_priv *priv = get_sfpiic_priv_const();
    if (!priv)
        return;
    printf("SFPIIC:  %s", sensor_status_ansi_str(get_sfpiic_sensor_status()));
    print_clear_eol();
}
