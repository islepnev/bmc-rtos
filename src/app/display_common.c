/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "display_common.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "devicelist.h"
#include "devicelist_print.h"
#include "display.h"

void display_devices(void)
{
    print_goto(2, 1);
    printf("Device list" ANSI_CLEAR_EOL "\n");
    devicelist_print(deviceList.list[0], 0);
    printf(ANSI_CLEAR_EOL);
}

void print_footer_line(void)
{
    printf(ANSI_BGR_BLUE ANSI_GRAY "SPACE: next page   P: switch power" ANSI_CLEAR_EOL ANSI_CLEAR);
}
