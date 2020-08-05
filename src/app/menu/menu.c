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
menu_item_t menu_1_3;

menu_item_t menu_2_1;
menu_item_t menu_2_2;
menu_item_t menu_2_3;

menu_item_t menu_3_1;

menu_item_t menu_1_1_1;
menu_item_t menu_1_1_2;
menu_item_t menu_1_1_3;

menu_item_t menu_1_2_1;
menu_item_t menu_1_2_2;
menu_item_t menu_1_2_3;
menu_item_t menu_1_2_4;

menu_item_t menu_1_2_3_1;

const menu_item_t *menu_home = &menu;
const menu_item_t *menu_current = &menu_1;

void init_menu(void)
{
    menu.command = 0;
    menu.label = "";
    menu.text = "Main menu";
    menu.children = &menu_1;

    menu_1.parent = &menu;
    menu_1.command = 1;
    menu_1.label = "1";
    menu_1.text = "One";
    menu_1.next = &menu_2;
    menu_2.parent = &menu;
    menu_2.command = 2;
    menu_2.label = "2";
    menu_2.text = "Two";
    menu_2.next = &menu_3;
    menu_3.parent = &menu;
    menu_3.command = 3;
    menu_3.label = "3";
    menu_3.text = "Three";
    menu_3.next = &menu_1;

    menu_1.children = &menu_1_1;
    menu_1_1.parent = &menu_1;
    menu_1_1.command = 101;
    menu_1_1.label = "1";
    menu_1_1.text = "One One";
    menu_1_1.next = &menu_1_2;
    menu_1_2.parent = &menu_1;
    menu_1_2.command = 102;
    menu_1_2.label = "2";
    menu_1_2.text = "One Two";
    menu_1_2.next = &menu_1_3;
    menu_1_3.parent = &menu_1;
    menu_1_3.command = 103;
    menu_1_3.label = "3";
    menu_1_3.text = "One Tree";
    menu_1_3.next = &menu_1_1;

    menu_2.children = &menu_2_1;
    menu_2_1.parent = &menu_2;
    menu_2_1.command = 201;
    menu_2_1.label = "1";
    menu_2_1.text = "Two One";
    menu_2_1.next = &menu_2_2;
    menu_2_2.parent = &menu_2;
    menu_2_2.command = 202;
    menu_2_2.label = "2";
    menu_2_2.text = "Two Two";
    menu_2_2.next = &menu_2_3;
    menu_2_3.parent = &menu_2;
    menu_2_3.command = 203;
    menu_2_3.label = "3";
    menu_2_3.text = "Two Tree";
    menu_2_3.next = &menu_2_1;

    menu_3.children = &menu_3_1;
    menu_3_1.parent = &menu_3;
    menu_3_1.command = 301;
    menu_3_1.label = "1";
    menu_3_1.text = "Three One";
    menu_3_1.next = 0;

    menu_1_1.children = &menu_1_1_1;
    menu_1_1_1.parent = &menu_1_1;
    menu_1_1_1.command = 10101;
    menu_1_1_1.label = "1";
    menu_1_1_1.text = "One One One";
    menu_1_1_1.next = &menu_1_1_2;
    menu_1_1_2.parent = &menu_1_1;
    menu_1_1_2.command = 10102;
    menu_1_1_2.label = "2";
    menu_1_1_2.text = "One One Two";
    menu_1_1_2.next = &menu_1_1_3;
    menu_1_1_3.parent = &menu_1_1;
    menu_1_1_3.command = 10103;
    menu_1_1_3.label = "3";
    menu_1_1_3.text = "One One Three";
    menu_1_1_3.next = &menu_1_1_1;

    menu_1_2.children = &menu_1_2_1;
    menu_1_2_1.parent = &menu_1_2;
    menu_1_2_1.command = 10201;
    menu_1_2_1.label = "1";
    menu_1_2_1.text = "One Two One";
    menu_1_2_1.next = &menu_1_2_2;
    menu_1_2_2.parent = &menu_1_2;
    menu_1_2_2.command = 10202;
    menu_1_2_2.label = "2";
    menu_1_2_2.text = "One Two Two";
    menu_1_2_2.next = &menu_1_2_3;
    menu_1_2_3.parent = &menu_1_2;
    menu_1_2_3.command = 10203;
    menu_1_2_3.label = "3";
    menu_1_2_3.text = "One Two Three";
    menu_1_2_3.next = &menu_1_2_4;
    menu_1_2_4.parent = &menu_1_2;
    menu_1_2_4.command = 10204;
    menu_1_2_4.label = "4";
    menu_1_2_4.text = "One Two Four";
    menu_1_2_4.next = &menu_1_2_1;

    menu_1_2_3.children = &menu_1_2_3_1;
    menu_1_2_3_1.parent = &menu_1_2_3;
    menu_1_2_3_1.command = 1020301;
    menu_1_2_3_1.label = "1";
    menu_1_2_3_1.text = "One Two Three One";
    menu_1_2_3_1.next = 0;
}
