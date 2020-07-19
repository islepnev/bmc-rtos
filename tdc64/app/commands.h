/*
**    Copyright 2019 Ilja Slepnev
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
#ifndef COMMANDS_H
#define COMMANDS_H

#include "cmsis_os.h"

typedef enum {
    COMMAND_POTS_RESET = 1,
    COMMAND_POTS_INC,
    COMMAND_POTS_DEC,
    COMMAND_POTS_WRITE
} command_id_t;

typedef struct {
    command_id_t command_id;
    uint8_t arg;
} CommandPots;

osMailQId mq_cmd_pots_id;

void commands_init(void);

#endif // COMMANDS_H
