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

#ifndef DEV_DIGIPOT_COMMANDS
#define DEV_DIGIPOT_COMMANDS

#include "dev_digipot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CommandDigipots;
void digipot_process_command(Dev_digipots_priv *d, const struct CommandDigipots *cmd);
void digipot_check_mail(Dev_digipots_priv *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_DIGIPOT_COMMANDS
