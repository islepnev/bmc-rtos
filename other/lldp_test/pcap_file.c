/*
**    Copyright 2011-2021 Ilia Slepnev
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

#include "pcap_file.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

const uint32_t PCAP_MAGIC = 0xa1b2c3d4;

void write_pcap(const void *buf, size_t size, const char *outputFileName)
{
    FILE *outf = fopen(outputFileName, "wb");
    if (!outf) {
        fprintf(stderr, "Error writing file '%s': %s\n", outputFileName, strerror(errno));
        return;
    }
    struct pcap_file_header pcap_hdr;
    pcap_hdr.magic = PCAP_MAGIC;
    pcap_hdr.version_major = 2;
    pcap_hdr.version_minor = 4;
    pcap_hdr.thiszone = 0;
    pcap_hdr.sigfigs = 0;
    pcap_hdr.snaplen = size;
    pcap_hdr.linktype = 1;

    if (1 != fwrite((const char *)&pcap_hdr, sizeof(pcap_hdr), 1, outf)) {
        fprintf(stderr, "Error writing file '%s': %s\n", outputFileName, strerror(errno));
        goto err;
    }
    {
        uint32_t caplen = size;
        uint32_t len = caplen;
        uint32_t tv_sec = 0;
        uint32_t tv_usec = 0;

        if (1 != fwrite((const char *)&tv_sec, 4, 1, outf)) goto err;
        if (1 != fwrite((const char *)&tv_usec, 4, 1, outf)) goto err;
        if (1 != fwrite((const char *)&caplen, 4, 1, outf)) goto err;
        if (1 != fwrite((const char *)&len, 4, 1, outf)) goto err;
        if (1 != fwrite(buf, size, 1, outf)) goto err;
    }

    fclose(outf);
    return;
err:;
    fprintf(stderr, "Error writing file '%s': %s\n", outputFileName, strerror(errno));
    fclose(outf);
}
