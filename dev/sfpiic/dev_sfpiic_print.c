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

const int STALE_TIMEOUT_TICKS = 5000;

const char *extended_compliance_str_qsfp(uint8_t value)
{
    switch (value) {
    case 0: return "";
    case 1: return "100G AOC";
    case 2: return "100GBASE-SR4";
    case 3: return "100GBASE-LR4";
    case 4: return "100GBASE-ER4";
    case 0x11: return "4 x 10GBASE-SR";
    case 0x12: return "40G PSM4";
    default: return "other";
    };
}

const char *compliance_10_40_100_str(uint8_t value)
{
    switch (value) {
    case 0x40: return "10GBASE-LRM";
    case 0x20: return "10GBASE-LR";
    case 0x10: return "10GBASE-SR";
    case 0x08: return "40GBASE-CR4";
    case 0x04: return "40GBASE-SR4";
    case 0x02: return "40GBASE-LR4";
    case 0x01: return "40G Active";
    default: return "other 10G/40G";
    }
}

const char *compliance_1g_str(uint8_t value)
{
    switch (value) {
    case 0x80: return "BASE-PX";
    case 0x40: return "BASE-BX10";
    case 0x20: return "100BASE-FX";
    case 0x10: return "100BASE-LX/LX10";
    case 0x08: return "1000BASE-T";
    case 0x04: return "1000BASE-CX";
    case 0x02: return "1000BASE-LX";
    case 0x01: return "1000BASE-SX";
    default: return "other 1G";
    }
}

void print_extended_compliance_str(const sfpiic_ch_status_t *status)
{
    printf("%s ", extended_compliance_str_qsfp(status->compliance_extended));
}

const char *ethernet_compliance_str(const sfpiic_ch_status_t *status)
{
    const uint8_t id3 = status->compliance_ethernet_10_40_100;
    const uint8_t id6 = status->compliance_ethernet_1g;
    if (id3 & (1 << 7))
        return extended_compliance_str_qsfp(status->compliance_extended);
    if (id3 != 0)
        return compliance_10_40_100_str(status->compliance_ethernet_10_40_100);
    if (id6 != 0)
        return compliance_1g_str(status->compliance_ethernet_1g);
    return "unknown";
}

void print_ethernet_compliance_str(const sfpiic_ch_status_t *status)
{
    const uint8_t id3 = status->compliance_ethernet_10_40_100;
    const uint8_t id6 = status->compliance_ethernet_1g;

    if (id3 & (1 << 7))
        print_extended_compliance_str(status);
    if (id3 & (1 << 6))
        printf("10GBASE-LRM ");
    if (id3 & (1 << 5))
        printf("10GBASE-LR ");
    if (id3 & (1 << 4))
        printf("10GBASE-SR ");
    if (id3 & (1 << 3))
        printf("40GBASE-CR4 ");
    if (id3 & (1 << 2))
        printf("40GBASE-SR4 ");
    if (id3 & (1 << 1))
        printf("40GBASE-LR4 ");
    if (id3 & (1 << 0))
        printf("40G Active ");

    if (id6 & (1 << 7))
        printf("BASE-PX ");
    if (id6 & (1 << 6))
        printf("BASE-BX10 ");
    if (id6 & (1 << 5))
        printf("100BASE-FX ");
    if (id6 & (1 << 4))
        printf("100BASE-LX/LX10 ");
    if (id6 & (1 << 3))
        printf("1000BASE-T ");
    if (id6 & (1 << 2))
        printf("1000BASE-CX ");
    if (id6 & (1 << 1))
        printf("1000BASE-LX ");
    if (id6 & (1 << 0))
        printf("1000BASE-SX ");

    if (id3 == 0 && id6 == 0)
        printf("unsupported");
}

const char *dev_sfpiic_transceiver_str(uint8_t value)
{
    switch (value) {
    case SFF8024_ID_SFP: return "SFP";
    case SFF8024_ID_QSFP: return "QSFP";
    case SFF8024_ID_QSFP28: return "QSFP28";
    case SFF8024_ID_QSFP_PLUS: return "QSFP+";
    case SFF8024_ID_CXP: return "CXP";
    default: return "unknown";
    }
}

const char *dev_sfpiic_connector_str(uint8_t value)
{
    switch (value) {
    case SFF8024_CTOR_MPO: return "MPO";
    case SFF8024_CTOR_LC: return "LC";
    case SFF8024_CTOR_RJ45: return "RJ45";
    case SFF8024_CTOR_NO_SEPARABLE: return "none";
    default: return "unknown";
    }
}

static void sfpPrintSummaryHeader(void)
{
    printf("%-6s  %-6s  %-4s  %-11s  %-16s %-16s %-16s %5s  %5s  %6s  %6s  %6s",
           "Port", // 6
           "Type", // 6
           "Conn", // 4
           "Compliance", // 11
           "Vendor_name", // 16
           "Vendor_part", // 16
           "Vendor_serial", // 16
           "Temp",  // 5
           "Volt", // 5
           "Rx dBm", // 6
           "Tx dBm", // 6
           "Bias" // 6
           );
    print_clear_eol();
}

void sfpPrintSummaryStatus(const Dev_sfpiic_priv *d)
{
    const uint32_t now = osKernelSysTick();
    sfpPrintSummaryHeader();
    print_clear_eol();
    for (uint32_t i=0; i<d->portCount; i++) {
        const int portIndex = d->portIndex[i];
        const sfpiic_ch_status_t *status = &d->status.sfp[portIndex];
        printf("%-6s  ", d->portName[portIndex]);
        if (!status->present) {
            print_clear_eol();
            continue;
        }
        switch (status->ch_state) {
        case SFPIIC_CH_STATE_READY: {
            bool id_updated = (uint32_t)(now - status->id_updated_timetick) < STALE_TIMEOUT_TICKS;
            bool dom_updated = (uint32_t)(now - status->dom_updated_timetick) < STALE_TIMEOUT_TICKS;
            printf("%s%-6s  %-4s  %-11s  %-16s %-16s %-16s ",
                   id_updated ? "" : ANSI_RED,
                   dev_sfpiic_transceiver_str(status->transceiver),
                   dev_sfpiic_connector_str(status->connector),
                   ethernet_compliance_str(status),
                   status->vendor_name, status->vendor_pn, status->vendor_serial
                   );
            if (status->dom_supported) {
                printf("%s% 5.1f  %5.2f  ",
                       dom_updated ? ANSI_CLEAR : ANSI_RED,
                       status->temp, status->voltage
                       );
                double sum_rx_power = 0;
                double sum_tx_power = 0;
                double sum_bias_cur = 0;
                int lane_count = d->multilane[portIndex] ? 4 : 1;
                for (int l=0; l < lane_count; l++) {
                    sum_rx_power += status->rx_power_mw[l];
                    sum_tx_power += status->tx_power_mw[l];
                    sum_bias_cur += status->bias_cur[l];
                }
                sum_rx_power /= lane_count;
                sum_tx_power /= lane_count;
                sum_bias_cur /= lane_count;
                printf("%6.2f  %6.2f  %6.2f",
                       convert_mw_to_dbm(sum_rx_power),
                       convert_mw_to_dbm(sum_tx_power),
                       sum_bias_cur
                       );
            }
            printf("  ");
            printf(ANSI_CLEAR);
            break;
        }
        case SFPIIC_CH_STATE_UNKNOWN:
            printf(" STATE_UNKNOWN");
            break;
        case SFPIIC_CH_STATE_ERROR:
            printf(" STATE_ERROR");
            break;
        }
        print_clear_eol();
    }
}

void sfpPrintChannelHeader(void)
{
    printf("  Port  Lane  Rx dBm  Tx dBm    Bias");
    print_clear_eol();
}

static void sfpPrintChannelStatus(const Dev_sfpiic_priv *d)
{
    const uint32_t now = osKernelSysTick();
    int print_header = 1;
    int skip_count = 0;
    for (int i=0; i<SFPIIC_CH_CNT; i++) {
        const int portIndex = d->portIndex[i];
        const sfpiic_ch_status_t *status = &d->status.sfp[portIndex];
        if (!d->multilane[portIndex])
            continue;
        if (!status->present) {
            skip_count++;
            continue;
        }
        if(print_header) {
            sfpPrintChannelHeader();
            print_header=0;
        } else {
            printf("-------------------------------------");
            print_clear_eol();
        }
        bool dom_updated = (uint32_t)(now - status->dom_updated_timetick) < STALE_TIMEOUT_TICKS;
        for (int ch=0; ch<4; ch++) {
            printf(" %-6s   %1d   %s%6.2f  %6.2f  %6.2f",
                   d->portName[portIndex], ch+1,
                   dom_updated ? ANSI_CLEAR : ANSI_RED,
                   convert_mw_to_dbm(status->rx_power_mw[ch]),
                   convert_mw_to_dbm(status->tx_power_mw[ch]),
                                     status->bias_cur[ch]
                   );
            printf(ANSI_CLEAR);
            print_clear_eol();
        }
    }
    // clear screen area
    for (int i=0; i<skip_count; i++) {
        for (int j=0; j<5; j++)
            print_clear_eol();
    }
}

void sfpPrintStatus(void)
{
    const Dev_sfpiic_priv *priv = get_sfpiic_priv_const();
    if (!priv)
        return;
    print_clear_eol();
    // sff8079_show_all(priv->status.sfp[3].idprom);
    // sff8472_show_all(priv->status.sfp[3].diag);

    sfpPrintSummaryStatus(priv);
    print_clear_eol();
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
