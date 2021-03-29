/*
**    Copyright 2019-2021 Ilja Slepnev
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

#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sntp_client_init(void);

extern void sntp_client_set_system_time(unsigned int sec, unsigned int microsec);

#ifdef __cplusplus
}
#endif

#endif /* SNTP_CLIENT_H */
