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

#include "menu.h"

#include <assert.h>

#include "menu_item.h"
#include "menu_tree.h"

menu_item_t menu = {0};

menu_item_t menu_1;
menu_item_t menu_2;
menu_item_t menu_3;

menu_item_t menu_1_1;
menu_item_t menu_1_2;
menu_item_t menu_1_3;

menu_item_t menu_2_1;
menu_item_t menu_2_2;
menu_item_t menu_2_3;

menu_item_t menu_3_1;
menu_item_t menu_3_2;
menu_item_t menu_3_3;

menu_item_t menu_1_1_1;
menu_item_t menu_1_1_2;
menu_item_t menu_1_1_3;

menu_item_t menu_1_2_1;
menu_item_t menu_1_2_2;
menu_item_t menu_1_2_3;

menu_item_t menu_1_3_1;
menu_item_t menu_1_3_2;
menu_item_t menu_1_3_3;

menu_item_t menu_1_2_1_1;
menu_item_t menu_1_2_1_2;
menu_item_t menu_1_2_1_3;

menu_item_t menu_1_2_2_1;
menu_item_t menu_1_2_2_2;
menu_item_t menu_1_2_2_3;

menu_item_t menu_1_2_3_1;
menu_item_t menu_1_2_3_2;
menu_item_t menu_1_2_3_3;

const menu_item_t *menu_home = &menu;
const menu_item_t *menu_current = &menu_1;

menu_item_t *add_menu_item(menu_item_t *parent, menu_item_t *item, int command, const char *label, const char *text)
{
    assert(item);
    if (!item)
        return item;
    item->parent = parent;
    item->command = command;
    item->label = label;
    item->text = text;
    append_child(parent, item);
    return item;
}

void init_menu_0(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1, 1, "1", "One");
    add_menu_item(parent, &menu_2, 2, "2", "Two");
    add_menu_item(parent, &menu_3, 3, "3", "Three");
}

void init_menu_1(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_1, 101, "1", "One-One");
    add_menu_item(parent, &menu_1_2, 102, "2", "One-Two");
    add_menu_item(parent, &menu_1_3, 103, "3", "One-Three");
}

void init_menu_2(menu_item_t *parent)
{
    add_menu_item(parent, &menu_2_1, 201, "1", "Two-One");
    add_menu_item(parent, &menu_2_2, 202, "2", "Two-Two");
    add_menu_item(parent, &menu_2_3, 203, "3", "Two-Three");
}

void init_menu_3(menu_item_t *parent)
{
    add_menu_item(parent, &menu_3_1, 301, "1", "Three-One");
    add_menu_item(parent, &menu_3_2, 302, "2", "Three-Two");
    add_menu_item(parent, &menu_3_3, 303, "3", "Three-Three");
}

void init_menu_1_1(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_1_1, 10101, "1", "One-One-One");
    add_menu_item(parent, &menu_1_1_2, 10102, "2", "One-One-Two");
    add_menu_item(parent, &menu_1_1_3, 10103, "3", "One-One-Three");
}

void init_menu_1_2(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_2_1, 10201, "1", "One-Two-One");
    add_menu_item(parent, &menu_1_2_2, 10202, "2", "One-Two-Two");
    add_menu_item(parent, &menu_1_2_3, 10203, "3", "One-Two-Three");
}

void init_menu_1_3(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_3_1, 10301, "1", "One-Three-One");
    add_menu_item(parent, &menu_1_3_2, 10302, "2", "One-Three-Two");
    add_menu_item(parent, &menu_1_3_3, 10303, "3", "One-Three-Three");
}

void init_menu_1_2_1(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_2_1_1, 1020101, "1", "One-Two-One-One");
    add_menu_item(parent, &menu_1_2_1_2, 1020102, "2", "One-Two-One-Two");
    add_menu_item(parent, &menu_1_2_1_3, 1020103, "3", "One-Two-One-Three");
}

void init_menu_1_2_2(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_2_2_1, 1020201, "1", "One-Two-Two-One");
    add_menu_item(parent, &menu_1_2_2_2, 1020202, "2", "One-Two-Two-Two");
    add_menu_item(parent, &menu_1_2_2_3, 1020203, "3", "One-Two-Two-Three");
}

void init_menu_1_2_3(menu_item_t *parent)
{
    add_menu_item(parent, &menu_1_2_3_1, 1020301, "1", "One-Two-Three-One");
    add_menu_item(parent, &menu_1_2_3_2, 1020302, "2", "One-Two-Three-Two");
    add_menu_item(parent, &menu_1_2_3_3, 1020303, "3", "One-Two-Three-Three");
}

void init_menu(void)
{
    menu.command = 0;
    menu.label = "";
    menu.text = "Main menu";

    init_menu_0(&menu);

    init_menu_1(&menu_1);
    init_menu_2(&menu_2);
    init_menu_3(&menu_3);

    init_menu_1_1(&menu_1_1);
    init_menu_1_2(&menu_1_2);
    init_menu_1_3(&menu_1_3);

    init_menu_1_2_1(&menu_1_2_1);
    init_menu_1_2_2(&menu_1_2_2);
    init_menu_1_2_3(&menu_1_2_3);
}
