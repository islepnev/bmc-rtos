/*
**    Copyright 2020 Ilia Slepnev
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

#include "str_util.h"

#include <string.h>

void trim_quotes(char *str)
{
    const size_t len = strlen(str);
    if (len < 2)
        return;
    const char *firstchar = &str[0];
    const char *lastchar = &str[len-1];
    if (*firstchar != *lastchar)
        return;
    if (*firstchar == '\"' || *firstchar == '\'') {
        for (size_t i=1; i<len; i++)
            str[i-1] = str[i];
        str[len-2] = '\0';
    }
}

void trim_eol(char *str)
{
    while (strlen(str) > 0) {
        char *lastchar = &str[strlen(str)-1];
        if (*lastchar == '\r' || *lastchar == '\n')
            *lastchar = '\0';
        else
            break;
    }
}
