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
#include "menu/menu.h"
#include "menu/menu_exec.h"
#include "menu/menu_tree.h"
#include "log/log.h"

static void set_current_item(const menu_item_t *item)
{
    if (item == menu_current)
        return;
    menu_current = item;
    schedule_display_refresh();
}

static void menu_process_command(const CommandMenu *cmd)
{
    const menu_item_t *item = menu_current;

    switch (cmd->command_id) {
    case COMMAND_MENU_DIGIT_0:
    case COMMAND_MENU_DIGIT_1:
    case COMMAND_MENU_DIGIT_2:
    case COMMAND_MENU_DIGIT_3:
    case COMMAND_MENU_DIGIT_4:
    case COMMAND_MENU_DIGIT_5:
    case COMMAND_MENU_DIGIT_6:
    case COMMAND_MENU_DIGIT_7:
    case COMMAND_MENU_DIGIT_8:
    case COMMAND_MENU_DIGIT_9:
        item = find_nth_sibling(item, cmd->command_id - COMMAND_MENU_DIGIT_0);
        break;
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
        else
            menu_exec(item);
        break;
    case COMMAND_MENU_BACK:
        item = find_previous_level_menu(item);
        break;
    case COMMAND_MENU_HOME:
        item = menu_home->children;
        break;
    default:;
    }
    set_current_item(item);
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


static void print_menu_level(const menu_item_t *item)
{
    const menu_item_t *cur = item;
    const int level = menu_level(item);
    // printf("L%d ", level);
    for (int i=1; i<=level; i++) {
        const menu_item_t *p = find_nth_level_parent(item, i);
        printf("%s%s", (i>1) ? "." : "", p->label);
    }
}

void display_menu_page(int y, bool repaint)
{
    menu_check_mail();

    if (repaint)
        print_clearbox(y, screen_height - 2);

    print_goto(y, 1);
    print_clear_eol(); y++;
    assert(menu_current);
    const menu_item_t *parent = menu_current->parent;
    assert(parent);

    print_menu_level(parent);
    printf(" %s", parent->text);
    print_clear_eol(); y++;

    const menu_item_t *start_item = parent->children;
    assert(start_item);
    int n = 0;
    const menu_item_t *item = start_item;
    while (item) {
        n++;
        bool hilight = (menu_current == item);
        printf("%s", hilight ? ANSI_BGR_RED : ANSI_BGR_DEF);
        print_menu_level(item);
        printf(" (%d)  %s%s" ANSI_BGR_DEF ANSI_CLEAR_EOL "\n",
               item->command, item->text, item->children ? "..." : "");
        item = item->next;
        if (item == start_item)
            break;
    }
    y += n;
    print_clearbox(y, screen_height - 1 - y);
}
