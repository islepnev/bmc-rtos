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

#include "display.h"

#include <stdio.h>
#include <string.h>

#include "log/log.h"

void print_goto(int line, int col)
{
    printf("\x1B[%d;%dH", line, col);
}

void print_clearbox(int line1, int height)
{
    printf("\x1B[%d;H", line1);
    for(int i=line1; i<line1+height; i++)
        printf("\x1B[K\n");
        // printf("\x1B[%d;H\x1B[K", i);
}

void print_get_screen_size(void)
{
    printf(CSI "r" CSI "999;999H"); // move to 999:999
    printf(CSI "6n");
    // repeat
    printf(CSI "r" CSI "999;999H"); // move to 999:999
    printf(CSI "6n");
}

static char printable(char c, char fill)
{
    if ((c < 0x20) || (c > 0x7e))
        return fill;
    return c;
}

enum {XD_LINE_BYTES = 16};
enum {XD_GROUPBY = 4};

static void hexdump_line(int address, const void *ptr, int len)
{
    if (!ptr || len <= 0 || len > XD_LINE_BYTES)
        return;
    enum { buflen = XD_LINE_BYTES * 2 + (XD_LINE_BYTES / XD_GROUPBY - 1) };
    char buf[buflen+1];
    buf[0] = 0;
    char asciibuf[XD_LINE_BYTES+1];
    asciibuf[0] = 0;
    for (int i=0; i<len; i++) {
        const char c = ((const char *)ptr)[i];
        asciibuf[i] = printable(c, '.');
        asciibuf[i+1] = 0;
        char str[4];
        snprintf(str, sizeof(str), "%02X", ((const char *)ptr)[i]);
        strncat(buf, str, buflen);
        if (i % XD_GROUPBY == XD_GROUPBY - 1) {
            strncat(buf, " ", buflen);
        }
    }
    for (int i=strlen(buf); i<buflen; i++)
        buf[i] = ' ';
    buf[buflen] = 0;

    log_printf(LOG_DEBUG, "  %04X: %s  %s", address, buf, asciibuf);
}

void hexdump(const void *ptr, int len)
{
    if (len <= 0) {
        log_printf(LOG_INFO, "hexdump: empty");
        return;
    }
    int i = 0;
    while (i <= len) {
        int start = i / XD_LINE_BYTES * XD_LINE_BYTES;
        int count = len - start;
        if (count > XD_LINE_BYTES)
            count = XD_LINE_BYTES;
        hexdump_line(start, ptr + i, count);
        i += XD_LINE_BYTES;
    }
}
