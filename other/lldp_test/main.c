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

#include <stdio.h>

#include "lldp_message.h"
#include "pcap_file.h"

enum { MAX_LLDP_LEN = 250 };

int main(int argc, char *argv[])
{
    const int maxbuflen = 14 + MAX_LLDP_LEN;
    uint8_t buf[maxbuflen];

    // ethernet header
    // dest mac
    buf[0] = 0x01;
    buf[1] = 0x80;
    buf[2] = 0xc2;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;
    // src mac
    buf[6] = 0x2;
    buf[7] = 0x3;
    buf[8] = 0x4;
    buf[9] = 0x5;
    buf[10] = 0x6;
    buf[11] = 0x7;
    // LLCP Ethertype 88CC
    buf[12] = 0x88;
    buf[13] = 0xcc;

    uint8_t *lldp_pdu_buf = &buf[14];
    lldp_info_t info = {
        .mac = "\x22\x33\x44\x55\x66\x77",
        .portdescr = "eth0",
        .sysname = "SysName",
        .sysdescr = "SysDescr",
        .ttl = 300,
        .ipv4 = 0x0A000001,
        .syscaps = 0x0080,
        // org specific
        .fw_rev = "fw 1.0",
        .hw_rev = "hw 1.0",
        .sw_rev = "sw 1.0",
        .serial = "12345678",
        .manuf = "Manufacturer",
        .model = "Model",
    };
    uint16_t lldp_pdu_size = lldp_create_pdu(lldp_pdu_buf, MAX_LLDP_LEN, &info);
    int buflen = 14 + lldp_pdu_size;

    for (int i=0; i<lldp_pdu_size; i++)
        printf("%02X ", lldp_pdu_buf[i]);

    printf("\n");
    write_pcap(buf, buflen, "lldp_test.pcap"); // tshark -r lldp_test.pcap -PV
    return 0;
}
