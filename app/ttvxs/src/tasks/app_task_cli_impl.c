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

void cycle_display_mode(void)
{
    switch (display_mode) {
    case DISPLAY_SUMMARY:
        display_mode = DISPLAY_BOARDS;
        break;
    case DISPLAY_BOARDS:
        display_mode = DISPLAY_SFP_DETAIL;
        break;
    case DISPLAY_SFP_DETAIL:
        display_mode = DISPLAY_PLL_DETAIL;
        break;
    case DISPLAY_PLL_DETAIL:
        display_mode = DISPLAY_DIGIPOT;
        break;
    case DISPLAY_DIGIPOT:
        display_mode = DISPLAY_LOG;
        break;
    case DISPLAY_LOG:
        display_mode = DISPLAY_TASKS;
        break;
    case DISPLAY_TASKS:
        display_mode = DISPLAY_DEVICES;
        break;
    case DISPLAY_DEVICES:
        display_mode = DISPLAY_SUMMARY;
        break;
    case DISPLAY_NONE:
        display_mode = DISPLAY_SUMMARY;
        break;
    default:
        break;
    }
    schedule_display_repaint();
}

void screen_handle_key(char ch)
{
    (void)ch;
}

const char *ESC_CODE_F1 = "OP";
const char *ESC_CODE_F2 = "OQ";
const char *ESC_CODE_F3 = "OR";
const char *ESC_CODE_F4 = "OS";
const char *ESC_CODE_F5 = "[15~";
const char *ESC_CODE_F6 = "[17~";
const char *ESC_CODE_F7 = "[18~";
const char *ESC_CODE_F8 = "[19~";
const char *ESC_CODE_F9 = "[20~";
const char *ESC_CODE_F10 = "[21~";
const char *ESC_CODE_F11 = "[23~";
const char *ESC_CODE_F12 = "[24~";

bool app_handle_escape_seq(const char *str)
{
    int old_display_mode = display_mode;
    bool handled = false;
    if (0 == strcmp(str, ESC_CODE_F5)) {
        display_mode = DISPLAY_SUMMARY;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F6)) {
        display_mode = DISPLAY_BOARDS;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F7)) {
        display_mode = DISPLAY_SFP_DETAIL;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F8)) {
        display_mode = DISPLAY_PLL_DETAIL;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F9)) {
        display_mode = DISPLAY_DIGIPOT;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F10)) {
        display_mode = DISPLAY_LOG;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F11)) {
        display_mode = DISPLAY_TASKS;
        handled = true;
    }
    if (0 == strcmp(str, ESC_CODE_F12)) {
        display_mode = DISPLAY_DEVICES;
        handled = true;
    }
    if (old_display_mode != display_mode)
        schedule_display_repaint();
    return handled;
}
