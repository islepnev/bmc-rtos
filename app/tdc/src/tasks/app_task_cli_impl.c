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

#include "app_shared_data.h"
#include "commands_digipot.h"

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
    if (DISPLAY_DIGIPOT == display_mode) {
        digipot_screen_handle_key(ch);
    }
}

bool app_handle_escape_seq(const char *str)
{
    if (display_mode == DISPLAY_DIGIPOT)
        return digipot_handle_escape_seq(str);
    return false;
}
