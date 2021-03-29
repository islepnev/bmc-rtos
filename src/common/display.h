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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "ansi_escape_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANSI_COL_RESULTS ANSI_CLEAR_EOL ANSI_COL60

#define STR_ON  ANSI_GREEN "ON " ANSI_CLEAR
#define STR_OFF ANSI_GRAY  "OFF" ANSI_CLEAR

#define STR_RESULT_ON  ANSI_COL_RESULTS STR_ON
#define STR_RESULT_OFF ANSI_COL_RESULTS STR_OFF

#define STR_RESULT_UNKNOWN ANSI_COL_RESULTS ANSI_GRAY   "UNKNOWN"  ANSI_CLEAR
#define STR_RESULT_NORMAL  ANSI_COL_RESULTS ANSI_GREEN  "NORMAL"   ANSI_CLEAR
#define STR_RESULT_WARNING ANSI_COL_RESULTS ANSI_YELLOW "WARNING"  ANSI_CLEAR
#define STR_RESULT_CRIT    ANSI_COL_RESULTS ANSI_RED    "CRITICAL" ANSI_CLEAR
#define STR_RESULT_FAIL    ANSI_COL_RESULTS ANSI_RED    "FAIL"     ANSI_CLEAR

void print_clearbox(int line1, int height);
void print_goto(int line, int col);
void print_get_screen_size(void);
void hexdump(const void *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
