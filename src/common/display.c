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

void print_clear_eol(void)
{
    printf("%s\n", ANSI_CLEAR_EOL);
}

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