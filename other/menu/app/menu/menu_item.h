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

#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include <stdbool.h>

#include "menu_command.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct menu_item_t {
    // menu_command_t command;
    unsigned int command;
    const char *label;
    const char *text;
    struct menu_item_t *parent;
    struct menu_item_t *prev;
    struct menu_item_t *next;
    struct menu_item_t *children;
} menu_item_t;

#ifdef __cplusplus
}
#endif

#endif // MENU_ITEM_H
