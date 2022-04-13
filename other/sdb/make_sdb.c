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
#include <strings.h>
#include <time.h>

#include "sdb.h"
#include "sdb_util.h"
#include "str_util.h"


static const uint64_t VENDOR_ID = 0x414649; // 'AFI'
static const uint16_t REGIO_FIRST_REG = 0;
static const uint16_t REGIO_LAST_REG = 0x7FFF;

typedef struct version_t {
    uint32_t v[4];
} version_t;

uint8_t device_id = 0;
char *board_name = "unknown";
version_t version = {{0, 0, 0, 0}};


enum mlink_bus_type {BUS_REGIO = 1, BUS_MEMIO = 2};

uint64_t dec2bcd(uint64_t d)
{
    uint64_t bcd = 0;
    int n = 0;
    while (d) {
        uint64_t digit = d % 10;
        bcd |= digit << n;
        d /= 10;
        n+=4;
    }
    return bcd;
}

uint32_t datecode()
{
    time_t now = time(0);
    struct tm tm;
    tm = *localtime(&now);
    uint32_t y = tm.tm_year+1900;
    uint32_t m = tm.tm_mon+1;
    uint32_t d = tm.tm_mday;
    uint32_t code = (dec2bcd(y) << 16) | (dec2bcd(m) << 8) | dec2bcd(d);
    return code;
}

bool parse_version(const char *str, version_t *v)
{
    int n = sscanf(str, "%d.%d.%d.%d",
                   &v->v[3], &v->v[2], &v->v[1], &v->v[0]);
    if (n < 1) {
        fprintf(stderr, "Version parse error: %s\n", str);
        return false;
    }
    return true;
}

uint32_t version_code(const version_t v)
{
    uint32_t code = (dec2bcd(v.v[3]) << 24) | (dec2bcd(v.v[2]) << 16) | (dec2bcd(v.v[1]) << 8) | dec2bcd(v.v[0]);
    return code;
}

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
    product->version   = version_code(version); // 0x01020304; // 1.2.3.4
    product->date      = datecode();
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
    product->version   = 0;
    product->date      = 0;
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

void print_meta()
{
    printf("Device ID: 0x%02X\n", device_id);
    printf("Board: '%s'\n", board_name);
    printf("Version: %d.%d.%d.%d\n", version.v[3], version.v[2], version.v[1], version.v[0]);
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

void parse_csv_comment(const char *s, size_t size)
{
    if (!s)
        return;
    {
        const char *key = "#ID=";
        if (strlen(s) > strlen(key) && 0 == strncasecmp(key, s, strlen(key))) {
            char *endptr = NULL;
            const char *sub = &s[strlen(key)];
            uint32_t n = strtol(sub, &endptr, 16);
            if (*endptr != '\0')
                fprintf(stderr, "Unable to parse Device ID '%s': %s\n", sub, endptr);
            else
                device_id = n;
        }
    }
    {
        const char *key = "#BOARD=";
        if (strlen(s) > strlen(key) && 0 == strncasecmp(key, s, strlen(key))) {
            const char *sub = &s[strlen(key)];
            board_name = malloc(strlen(s) + 1);
            strcpy(board_name, sub);
        }
    }
}

int read_csv(const char *filename, struct sdb_t *sdb)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return false;

    char linebuf[MAX_LINE_LENGTH];
    int linenumber = 0;
    int device_count = 0;
    while(fgets(linebuf, sizeof(linebuf), f)) {
        linenumber++;
        trim_eol(linebuf);
        if (strlen(linebuf) && linebuf[0] == '#') {
            parse_csv_comment(linebuf, MAX_LINE_LENGTH);
            continue;
        }
        if (!parse_line(filename, linenumber, linebuf, &sdb->device[device_count])) {
            break;
        }
        device_count++;
    }
    fclose(f);
    return device_count;
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
    fprintf(stderr, "USAGE: %s <description.csv> <sdb.bin> <sdb.ram_init.txt> [version]\n\n",
            argv[0]);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 4 || argc > 5)
        usage(argc, argv);

    const char *filename_csv = argv[1];
    const char *filename_bin = argv[2];
    const char *filename_txt = argv[3];
    if (argc >= 5) {
        parse_version(argv[4], &version);
    }

    if ((0 == strcmp(filename_csv, filename_bin)) ||
            (0 == strcmp(filename_csv, filename_txt)) ||
            (0 == strcmp(filename_bin, filename_txt)))
        usage(argc, argv);

    bool ok = true;
    struct sdb_t sdb;

//    fill_sdb_interconnect(&sdb.ic);
    int device_count = read_csv(filename_csv, &sdb);
    fill_sdb_interconnect(&sdb.ic);
    sdb.ic.sdb_records = 1 + device_count;
    print_meta();
    print_sdb(&sdb);
    ok &= write_sdb_bin(filename_bin, &sdb);
    ok &= write_sdb_ram_txt(filename_txt, &sdb);

    return ok ? 0 : 1;
}
