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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdb.h"
#include "sdb_util.h"
#include "str_util.h"


static const uint64_t VENDOR_ID = 0x414649; // 'AFI'
static const uint16_t REGIO_FIRST_REG = 0;
static const uint16_t REGIO_LAST_REG = 0x7FFF;
static const uint32_t DATECODE = 0; // 0x19861231; // 31 Dec 1986
static const uint32_t VERSIONCODE = 0;

static const uint8_t device_id = 0xCF; // CF aka TTVXS
static const char *board_name = "TTVXS";

enum mlink_bus_type {BUS_REGIO = 1, BUS_MEMIO = 2};

void fill_sdb_interconnect(struct sdb_interconnect *ic)
{
    ic->sdb_magic = (SDB_MAGIC);
    ic->sdb_records = (1);
    ic->sdb_version = 1;
    ic->sdb_bus_type = sdb_wishbone;
    struct sdb_component *component = &ic->sdb_component;
    component->addr_first = (REGIO_FIRST_REG << 1);
    component->addr_last  = ((REGIO_LAST_REG << 1) | 1);
    struct sdb_product *product = &component->product;
    product->vendor_id = VENDOR_ID;
    product->device_id = device_id;
    product->version   = VERSIONCODE; // 0x01020304; // 1.2.3.4
    product->date      = DATECODE;
    fill_sdb_string(product->name, sizeof(product->name), board_name);
    product->record_type = sdb_type_interconnect;
}

void fill_sdb_device(struct sdb_device *p,
                     enum mlink_bus_type bus,
                     uint32_t reg_base,
                     uint32_t reg_mask,
                     uint32_t device_id,
                     const char *name,
                     uint8_t ver_major,
                     uint8_t ver_minor)
{
    p->abi_class = 0;
    p->abi_ver_major = ver_major;
    p->abi_ver_minor = ver_minor;
    p->bus_specific = bus;
    struct sdb_component *component = &p->sdb_component;
    component->addr_first = (reg_base << 1);
    component->addr_last  = (((reg_base | reg_mask) << 1) | 1);
    struct sdb_product *product = &component->product;
    //    fill_sdb_product(&component->product, sdb_type_device, 0x4000, );
    product->vendor_id = (VENDOR_ID);
    product->device_id = (device_id);
    product->version   = VERSIONCODE; // 0x00010203; // 1.2.3
    product->date      = (DATECODE);
    fill_sdb_string(product->name, sizeof(product->name), name);
    product->record_type = sdb_type_device;
}

enum { MAX_SDB_DEVICE_COUNT = 32 };
struct sdb_t {
    struct sdb_interconnect ic;
    struct sdb_device device[MAX_SDB_DEVICE_COUNT];
};

enum { MAX_LINE_LENGTH = 256 };

typedef struct csv_line {
    uint32_t reg_base;
    uint32_t reg_mask;
    uint32_t device_id;
    char name[20];
} csv_line;

enum { REGIO_WORD_SIZE = 2 };

void print_sdb(struct sdb_t *dev)
{
    const struct sdb_interconnect *ic = &dev->ic;
    printf("SDB: %d records\n", ic->sdb_records);
    for (int i=0; i<ic->sdb_records - 1; i++) {
        if (i >= MAX_SDB_DEVICE_COUNT) {
            fprintf(stderr, "  <too many devices>\n");
            break;
        }
        struct sdb_device *d = &dev->device[i];
        if (!sdb_dev_validate(d)) {
            fprintf(stderr, "  <invalid record>\n");
        }
        char name[20];
        sdb_copy_printable(name, d->sdb_component.product.name, sizeof(d->sdb_component.product.name), ' ');
        name[19] = 0;
        printf("  %04x-%04x: %-20s %08X v%d.%d\n",
               (uint16_t)d->sdb_component.addr_first,
               (uint16_t)d->sdb_component.addr_last,
               name,
               d->sdb_component.product.device_id,
               d->abi_ver_major,
               d->abi_ver_minor
               );
    }
}

bool parse_line(const char *filename, int linenumber, const char *str, struct sdb_device *p)
{
    int x1, x2, x3, d5, d6;
    char name[MAX_LINE_LENGTH];
    int n = sscanf(str, "%x,%x,%x,%[^,],%d,%d",
                   &x1, &x2, &x3, name, &d5, &d6);
    if (6 != n) {
        fprintf(stderr, "%s:%d: Parse error after field %d\n", filename, linenumber, n);
        return false;
    }
    trim_quotes(name);
    fill_sdb_device(p, BUS_REGIO, x1, x2, x3, name, d5, d6);

    return true;
}

bool read_csv(const char *filename, struct sdb_t *sdb)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return false;

    bool ok = true;
    char linebuf[MAX_LINE_LENGTH];
    int linenumber = 0;
    int device_count = 0;
    while(fgets(linebuf, sizeof(linebuf), f)) {
        linenumber++;
        trim_eol(linebuf);
        if (strlen(linebuf) && linebuf[0] == '#')
            continue;
        if (!parse_line(filename, linenumber, linebuf, &sdb->device[device_count])) {
            ok = false;
            break;
        }
        device_count++;
    }
    fclose(f);
    sdb->ic.sdb_records = device_count + 1;
    return ok;
}

void sdb_fix_endian(struct sdb_t *p)
{
    assert(p->ic.sdb_magic == SDB_MAGIC);
    int device_count = p->ic.sdb_records - 1;
    sdb_interconnect_fix_endian(&p->ic);
    for (int i=0; i<device_count; i++) {
        if (i >= MAX_SDB_DEVICE_COUNT) {
            break;
        }
        struct sdb_device *d = &p->device[i];
        sdb_device_fix_endian(d);
    }
}

bool write_sdb_bin(const char *filename, const struct sdb_t *p)
{
    assert(p->ic.sdb_magic == SDB_MAGIC);
    struct sdb_t sdb_be;
    memcpy(&sdb_be, p, sizeof(sdb_be));

    int device_count = p->ic.sdb_records - 1;

    sdb_fix_endian(&sdb_be);
    const int sdb_size = sizeof(struct sdb_interconnect) + device_count * sizeof(struct sdb_device);
    {
        // write binary file
        FILE *f = fopen(filename, "w");
        if (!f)
            goto err;

        if (fwrite(&sdb_be, sdb_size, 1, f) != 1)
            goto err;
        if (fclose(f) != 0)
            goto err;
    }
    return true;
err:
    perror("error");
    return false;
}

bool write_sdb_ram_txt(const char *filename, const struct sdb_t *p)
{
    assert(p->ic.sdb_magic == SDB_MAGIC);
    struct sdb_t sdb_be;
    memcpy(&sdb_be, p, sizeof(sdb_be));

    int device_count = p->ic.sdb_records - 1;
    const int sdb_size = sizeof(struct sdb_interconnect) + device_count * sizeof(struct sdb_device);

    sdb_fix_endian(&sdb_be);

    const int sdb_words = sdb_size/2;
    {
        // write RAM init file
        FILE *f = fopen(filename, "w");
        if (!f)
            goto err;
        const uint16_t *sdb_alias = (const uint16_t *)&sdb_be;
        for (int i=0; i<sdb_words; i++)
            if (fprintf(f, "%04x\n", sdb_alias [i]) < 0)
                goto err;
        const int ram_words = 0x400;
        int pad_words = (ram_words - sdb_words);
        if (pad_words > 0) {
            for (int i=0; i<pad_words; i++)
                if (fprintf(f, "ffff\n") < 0)
                    goto err;
        }
        if (fclose(f) != 0)
            goto err;
    }
    return true;
err:
    perror("error");
    return false;
}

void usage(int argc, char *argv[])
{
    fprintf(stderr, "USAGE: %s <description.csv> <sdb.bin> <sdb.ram_init.txt>\n\n",
            argv[0]);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        usage(argc, argv);
    const char *filename_csv = argv[1];
    const char *filename_bin = argv[2];
    const char *filename_txt = argv[3];
    if ((0 == strcmp(filename_csv, filename_bin)) ||
            (0 == strcmp(filename_csv, filename_txt)) ||
            (0 == strcmp(filename_bin, filename_txt)))
        usage(argc, argv);

    bool ok = true;
    struct sdb_t sdb;

    fill_sdb_interconnect(&sdb.ic);
    ok &= read_csv(filename_csv, &sdb);
    print_sdb(&sdb);
    ok &= write_sdb_bin(filename_bin, &sdb);
    ok &= write_sdb_ram_txt(filename_txt, &sdb);

    return ok ? 0 : 1;
}
