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

#ifndef COMMANDS_MENU_H
#define COMMANDS_MENU_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool menu_handle_escape_seq(const char *str);
void menu_screen_handle_key(const char ch);

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_MENU_H
