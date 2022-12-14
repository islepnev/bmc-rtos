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

#include "app_task_cli_impl.h"

#include <string.h>

#include "app_shared_data.h"
#include "commands_digipot.h"
//#include "commands_menu.h"
#include "keysyms.h"

display_mode_t next_display_mode(display_mode_t mode)
{
    switch (mode) {
//    case DISPLAY_MENU:
//        return DISPLAY_SUMMARY;
    case DISPLAY_SUMMARY:
        return DISPLAY_BOARDS;
    case DISPLAY_BOARDS:
        return DISPLAY_SFP_DETAIL;
    case DISPLAY_SFP_DETAIL:
        return DISPLAY_PLL_DETAIL;
    case DISPLAY_PLL_DETAIL:
        return DISPLAY_DIGIPOT;
    case DISPLAY_DIGIPOT:
        return DISPLAY_LOG;
    case DISPLAY_LOG:
        return DISPLAY_TASKS;
    case DISPLAY_TASKS:
        return DISPLAY_DEVICES;
    case DISPLAY_DEVICES:
        return DISPLAY_SUMMARY;
    case DISPLAY_MODE_COUNT:
        return DISPLAY_SUMMARY;
    default:
        return DISPLAY_SUMMARY;
    }
}

display_mode_t prev_display_mode(display_mode_t mode)
{
    for (int i=0; i < DISPLAY_MODE_COUNT; i++)
        if (next_display_mode(i) == mode)
            return i;
    return mode;
}

void cycle_display_mode(void)
{
    display_mode = next_display_mode(display_mode);
    schedule_display_repaint();
}

void screen_handle_key(char ch)
{
//    if (DISPLAY_MENU == display_mode) {
//        menu_screen_handle_key(ch);
//    }
    if (DISPLAY_DIGIPOT == display_mode) {
        digipot_screen_handle_key(ch);
    }
}


bool app_handle_escape_seq(const char *str)
{
    display_mode_t old_display_mode = display_mode;
    bool handled = false;
    if (0 == strcmp(str, ESC_CODE_LEFT)) {
        display_mode = prev_display_mode(display_mode);
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_RIGHT)) {
        display_mode = next_display_mode(display_mode);
        handled = true;
    }
//    if (0 == strcmp(str, ESC_CODE_F1)) {
//        display_mode = DISPLAY_MENU;
//        handled = true;
//    }
    if (0 == strcmp(str, ESC_CODE_F2)) {
        display_mode = DISPLAY_SUMMARY;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F3)) {
        display_mode = DISPLAY_BOARDS;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F4)) {
        display_mode = DISPLAY_SFP_DETAIL;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F5)) {
        display_mode = DISPLAY_PLL_DETAIL;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F6)) {
        display_mode = DISPLAY_DIGIPOT;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F7)) {
        display_mode = DISPLAY_LOG;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F8)) {
        display_mode = DISPLAY_TASKS;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F9)) {
        display_mode = DISPLAY_DEVICES;
        handled = true;
    }

//    if (display_mode == DISPLAY_MENU) {
//        if (menu_handle_escape_seq(str)) {
//            handled = true;
//        }
//    }
    if (display_mode == DISPLAY_DIGIPOT) {
        if (digipot_handle_escape_seq(str)) {
            handled = true;
        }
    }
    if (old_display_mode != display_mode)
        schedule_display_repaint();
    return handled;
}
