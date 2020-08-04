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

menu_item_t menu = {0};

menu_item_t menu_1;
menu_item_t menu_2;
menu_item_t menu_3;

menu_item_t menu_1_1;
menu_item_t menu_1_2;

const menu_item_t *menu_home = &menu;
const menu_item_t *menu_current = &menu_1;

void init_menu(void)
{
    menu.command = 0;
    menu.children = &menu_1;

    menu_1.parent = &menu;
    menu_1.command = 1;
    menu_1.text = "1. One";
    menu_1.next = &menu_2;
    menu_2.parent = &menu;
    menu_2.command = 2;
    menu_2.text = "2. Two";
    menu_2.next = &menu_3;
    menu_3.parent = &menu;
    menu_3.command = 3;
    menu_3.text = "3. Three";
    menu_3.next = &menu_1;

    menu_1.children = &menu_1_1;
    menu_1_1.parent = &menu_1;
    menu_1_1.command = 101;
    menu_1_1.text = "1.1. One One";
    menu_1_1.next = &menu_1_2;
    menu_1_2.parent = &menu_1;
    menu_1_2.command = 102;
    menu_1_2.text = "1.2. Two Two";
    menu_1_2.next = &menu_1_1;

}
