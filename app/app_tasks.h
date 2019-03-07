//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef APP_TASKS_H
#define APP_TASKS_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

// for use with GCC flags -fstack-usage -Wstack-usage=N
enum { threadStackSize = 4000 };

enum {
    SIGNAL_REFRESH_DISPLAY = 1000,
    SIGNAL_POWER_ON = 1001,
    SIGNAL_POWER_OFF = 1002,
};
extern osThreadId powermonThreadId;
extern osThreadId displayThreadId;
extern osThreadId keyboardThreadId;

void create_tasks(void);

#ifdef __cplusplus
}
#endif

#endif // APP_TASKS_H
