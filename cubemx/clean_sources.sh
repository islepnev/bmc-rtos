#!/bin/bash

find Core Drivers LWIP Middlewares TrueSTUDIO \
-iname '*.h' -o -iname '*.c' -o -iname '*.s' | while read f; do
    dos2unix $f
    sed -i -e 's/[ \t]*$//' $f
done
