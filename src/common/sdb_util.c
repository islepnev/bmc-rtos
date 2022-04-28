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

#include "sdb_util.h"

#include <stdio.h>
#include <string.h>

#include "sdb.h"
#include "bswap.h"

void sdb_component_fix_endian(struct sdb_component *p)
{
    p->addr_first = ntohll(p->addr_first);
    p->addr_last = ntohll(p->addr_last);
    p->product.vendor_id = ntohll(p->product.vendor_id);
    p->product.device_id = ntohl(p->product.device_id);
    p->product.version = ntohl(p->product.version);
    p->product.date = ntohl(p->product.date);
}

void sdb_interconnect_fix_endian(struct sdb_interconnect *p)
{
    p->sdb_magic = ntohl(p->sdb_magic);
    p->sdb_records = ntohs(p->sdb_records);
    sdb_component_fix_endian(&p->sdb_component);
}

void sdb_synthesis_fix_endian(struct sdb_synthesis *p)
{
    p->tool_version = ntohl(p->tool_version);
    p->date = ntohl(p->date);
}

void sdb_device_fix_endian(struct sdb_device *p)
{
    p->abi_class = ntohl(p->abi_class);
    p->bus_specific = ntohl(p->bus_specific);
    sdb_component_fix_endian(&p->sdb_component);
}

bool sdb_ic_validate(const struct sdb_interconnect *p)
{
    if (p->sdb_magic != SDB_MAGIC)
        return false;
    if (p->sdb_version != 1)
        return false;
    if (p->sdb_component.product.record_type != sdb_type_interconnect)
        return false;
    if (p->sdb_records < 1)
        return false;
//    if (p->sdb_records > SDB_MAX_RECORDS)
//        return false;
    return true;
}

bool sdb_dev_validate(const struct sdb_device *p)
{
    if (p->abi_class != 0) {
        return false;
    }
    if (p->sdb_component.product.record_type != sdb_type_device) {
        return false;
    }
    return true;
}

void sdb_copy_printable(char *dest, const uint8_t *buf, size_t size, char fill)
{
    for (size_t i=0; i<size; i++) {
        char c = buf[i];
        if ((c < 0x20) || (c > 0x7e))
            *dest++ = fill;
        else
            *dest++ = c;
    }
    *dest = 0;
}

void fill_sdb_string(uint8_t *buf, uint8_t size, const char *str)
{
    size_t srclen = strlen(str);
    if (srclen > size)
        srclen = size;
    memcpy(buf, str, srclen);
    if (srclen < size)
        memset(&buf[srclen], 0, size - srclen);
}

static uint8_t bcd2bin(uint8_t value)
{
    uint32_t tmp = 0u;
    tmp = ((uint8_t)(value & (uint8_t)0xF0u) >> (uint8_t)0x4U) * 10u;
    return (tmp + (value & (uint8_t)0x0FU));
}

int snprint_sdb_version(char *str, size_t size, uint32_t version)
{
    if (!version) {
        if (str && size > 0)
            str[0] = '\0';
        return 0;
    }
    uint8_t v[4] = {
        bcd2bin((version >> 24) & 0xFF),
        bcd2bin((version >> 16) & 0xFF),
        bcd2bin((version >>  8) & 0xFF),
        bcd2bin((version) & 0xFF)
    };
    if (v[2] || v[3])
        return snprintf(str, size, "%d.%d.%d.%d", v[0], v[1], v[2], v[3]);
    if (v[2])
        return snprintf(str, size, "%d.%d.%d", v[0], v[1], v[2]);
    return snprintf(str, size, "%d.%d", v[0], v[1]);
}

int snprint_sdb_date(char *str, size_t size, uint32_t date)
{
    if (!date) {
        if (str && size > 0)
            str[0] = '\0';
        return 0;
    }
    uint8_t v[4] = {
        bcd2bin((date >> 24) & 0xFF),
        bcd2bin((date >> 16) & 0xFF),
        bcd2bin((date >>  8) & 0xFF),
        bcd2bin((date) & 0xFF)
    };
    return snprintf(str, size, "%02d.%02d.%02d%02d", v[3], v[2], v[0], v[1]);
}
