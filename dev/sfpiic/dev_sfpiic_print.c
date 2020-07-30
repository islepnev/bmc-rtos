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

#include "dev_sfpiic.h"
#include "dev_sfpiic_types.h"
#include "display.h"

void sfpPrintSummaryHeader(void)
{
    printf("\n"
           "Port "
           "Vendor_name    "
           "Vendor_serial  "
           "Temp "
           "Vol,mV "
           "\n");
}

void sfpPrintSummaryStatus(const Dev_sfpiic_priv *d)
{
    sfpPrintSummaryHeader();
    for(int row=0; row<SFPIIC_CH_CNT; ++row) {
        int sfp=0;
        if(row<3)
            sfp = 2-row;
        else
            sfp = SFPIIC_CH_CNT-row+2;

        const sfpiic_ch_status_t *status = &d->status.sfp[sfp];
        printf(" %-4d", sfp+1);
        if(!status->present) {
            printf(" ---\n");
            continue;
        }
        switch (status->ch_state) {
        case SFPIIC_CH_STATE_READY:
            printf("%-17s%-17s"
                   "%-5.1f"
                   "%-6.2f"
                   "\n"
                   ,status->vendor_name, status->vendor_serial
                   ,1./256*status->temp, 1e-4*status->volt
                   );
            break;
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
           "Rx,mW Tx,mW"
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
        if(print_header) {
            sfpPrintChannelHeader();
            print_header=0;
        } else {
            printf("--------------\n");

        }
        for(int ch=0; ch<4; ++ch) {
            printf(" %-4d%-3d"
                   "%-6d%-6d"
                   "\n"
                   , sfp+1, ch+1
                   , status->rx_pow[ch], status->tx_pow[ch]
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
