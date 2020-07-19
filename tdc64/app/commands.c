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
#include "commands.h"

osMailQDef(mq_cmd_pots, 10, CommandPots); // Declare a mail queue, size 10

void commands_init(void)
{
    mq_cmd_pots_id = osMailCreate(osMailQ(mq_cmd_pots), NULL);
    if (!mq_cmd_pots_id)
        Error_Handler();
}
