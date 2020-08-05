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

#ifndef MENU_TREE_H
#define MENU_TREE_H

#include <stdbool.h>

#include "menu.h"

#ifdef __cplusplus
extern "C" {
#endif

const menu_item_t *find_previous(const menu_item_t *item);
const menu_item_t *find_previous_level_menu(const menu_item_t *item);
const menu_item_t *find_nth_sibling(const menu_item_t *item, int n);
int menu_level(const menu_item_t *item);
const menu_item_t *find_nth_level_parent(const menu_item_t *item, int level);

#ifdef __cplusplus
}
#endif

#endif // MENU_TREE_H
