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

#include "display_menu.h"

#include <assert.h>
#include <stdio.h>

#include "app_shared_data.h"
#include "display.h"
#include "ipc.h"
#include "menu.h"

const menu_item_t *find_previous(const menu_item_t *item)
{
    const menu_item_t *tmp = item;
    while (tmp->next) {
        if (tmp->next == item) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return item;
}

static void menu_process_command(const CommandMenu *cmd)
{
    const menu_item_t *item = menu_current;

    switch (cmd->command_id) {
    case COMMAND_MENU_UP: {
        item = find_previous(item);
        break;
    }
    case COMMAND_MENU_DOWN:
        if (item->next)
            item = item->next;
        break;
    case COMMAND_MENU_ENTER:
        if (item->children)
            item = item->children;
        break;
    case COMMAND_MENU_BACK:
        if (item->parent && item->parent->parent)
            item = item->parent;
        break;
    case COMMAND_MENU_HOME:
        item = menu_home->children;
        break;
    default:;
    }
    menu_current = item;
}

static void menu_check_mail(void)
{
    assert(mq_cmd_menu_id);
    if (!mq_cmd_menu_id)
        return;
    osEvent event = osMailGet(mq_cmd_menu_id, 0);
    if (osEventMail != event.status) {
        return;
    }
    CommandMenu *mail = (CommandMenu *) event.value.p;
    assert(mail);
    if (!mail)
        return;
    menu_process_command(mail);
    osMailFree(mq_cmd_menu_id, mail);
}

void display_menu_page(int y, bool repaint)
{
    menu_check_mail();

    if (repaint)
        print_clearbox(y, screen_height - 1);

    print_goto(y, 1);
    const menu_item_t *parent = menu_current->parent;
    assert(parent);
    const menu_item_t *start_item = parent->children;
    assert(start_item);
    int n = 0;
    const menu_item_t *item = start_item;
    while (item) {
        n++;
        bool hilight = (menu_current == item);
        printf("%s%d.  (%d)  %s%s%s" ANSI_CLEAR_EOL "\n",
               hilight ? ANSI_BGR_RED : ANSI_BGR_DEF,
               n, item->command, item->text, item->children ? "..." : "",
               ANSI_BGR_DEF);
        item = item->next;
        if (item == start_item)
            break;
    }
    y += n;
    print_clearbox(y, screen_height - 1 - y);
}
