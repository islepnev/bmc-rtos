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

#ifndef APP_NAME_H
#define APP_NAME_H

#include "bsp.h"

#if defined(BOARD_CRU16)
#define APP_NAME_STR "CRU16 BMC"
#elif defined(BOARD_TDC64)
#define APP_NAME_STR "TDC64VHLE BMC"
#elif defined(BOARD_TDC72)
#define APP_NAME_STR "TDC72VXS4 BMC"
#elif defined(BOARD_TDC72VHLV3)
#define APP_NAME_STR "TDC72VHL-v3 BMC"
#elif defined(BOARD_TDC72VHLV2)
#define APP_NAME_STR "TDC72VHL-v2 BMC"
#elif defined(BOARD_TTVXS)
#define APP_NAME_STR "TTVXS BMC"
#else
#error Unknown BOARD
#endif

#endif // APP_NAME_H
