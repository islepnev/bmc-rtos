#!/bin/sh

gcc -std=c99 \
-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -W \
make_sdb.c -I ../../src/common ../../src/common/sdb_crc16.c ../../src/common/sdb_util.c ../../src/common/str_util.c -o make_sdb
