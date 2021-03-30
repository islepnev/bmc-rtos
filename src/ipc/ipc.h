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

#ifndef IPC_H
#define IPC_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COMMAND_DIGIPOTS_RESET = 1,
    COMMAND_DIGIPOTS_INC,
    COMMAND_DIGIPOTS_DEC,
    COMMAND_DIGIPOTS_WRITE
} digipot_command_id_t;

typedef struct CommandDigipots {
    digipot_command_id_t command_id;
    uint8_t arg;
} CommandDigipots;

extern osMailQId mq_cmd_digipots_id;


typedef enum {
    COMMAND_MENU_HOME = 1,
    COMMAND_MENU_DIGIT_0,
    COMMAND_MENU_DIGIT_1,
    COMMAND_MENU_DIGIT_2,
    COMMAND_MENU_DIGIT_3,
    COMMAND_MENU_DIGIT_4,
    COMMAND_MENU_DIGIT_5,
    COMMAND_MENU_DIGIT_6,
    COMMAND_MENU_DIGIT_7,
    COMMAND_MENU_DIGIT_8,
    COMMAND_MENU_DIGIT_9,
    COMMAND_MENU_UP,
    COMMAND_MENU_DOWN,
    COMMAND_MENU_BACK,
    COMMAND_MENU_ENTER
} menu_command_id_t;

typedef struct CommandMenu {
    menu_command_id_t command_id;
    uint8_t arg;
} CommandMenu;

extern osMailQId mq_cmd_menu_id;

#ifdef __cplusplus
}
#endif

#endif // IPC_H
