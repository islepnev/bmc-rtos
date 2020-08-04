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

display_mode_t next_display_mode(display_mode_t mode)
{
    switch (mode) {
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
    (void)ch;
}

const char *ESC_CODE_LEFT = "[D";
const char *ESC_CODE_RIGHT = "[C";
const char *ESC_CODE_UP = "[A";
const char *ESC_CODE_DOWN = "[B";

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
    if (old_display_mode != display_mode)
        schedule_display_repaint();
    return handled;
}
