#!/bin/sh

gcc -std=c99 -Wall make_sdb.c -I ../../src/common ../../src/common/sdb_util.c ../../src/common/str_util.c -o make_sdb
