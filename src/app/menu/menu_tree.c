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

#include "menu_tree.h"

#include <assert.h>

menu_item_t *append_child(menu_item_t *parent, menu_item_t *item)
{
    assert(parent);
    if (!parent)
        return item;
    if (parent->children) {
        // last element of maybe circular list
        menu_item_t *start = parent->children;
        menu_item_t *p = start;
        while (p && p->next) {
            p = p->next;
            if (p == start)
                break; // loop detected
        }
        menu_item_t *orig_last = p;
        p->next = item;
        item->prev = orig_last;
    } else {
        parent->children = item;
    }
    return item;
}

const menu_item_t *find_previous_level_menu(const menu_item_t *item)
{
    return (item->parent  && item->parent->parent)? item->parent : item;
}

const menu_item_t *find_nth_sibling(const menu_item_t *item, int n)
{
    assert(item);
    if (!item)
        return item;
    if (n == 0)
        return find_previous_level_menu(item);
    assert(item->parent);
    if (!item->parent)
        return item;
    // find first child
    assert(item->parent->children);
    const menu_item_t *start = item->parent->children;
    const menu_item_t *cur = start;
    int i = 1;
    while (cur) {
        if (i == n)
            return cur;
        cur = cur->next;
        if (cur == start)
            break; // loop detected
    }
    return item;
}

int menu_level(const menu_item_t *item)
{
    int level = 0;
    const menu_item_t *cur = item;
    while (cur && cur->parent) {
        cur = cur->parent;
        level++;
    }
    return level;
}

const menu_item_t *find_nth_level_parent(const menu_item_t *item, int level)
{
    const menu_item_t *cur = item;
    while (cur) {
        if (menu_level(cur) == level)
            return cur;
        cur = cur->parent;
    }
    return item;
}
