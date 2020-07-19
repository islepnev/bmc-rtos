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
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef APP_TASK_MAIN_H
#define APP_TASK_MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MAIN_STATE_INIT,
    MAIN_STATE_DETECT,
    MAIN_STATE_RUN,
    MAIN_STATE_ERROR
} MainState;

MainState getMainState(void);
uint32_t getMainLoopCount(void);
void create_task_main(void);

#ifdef __cplusplus
}
#endif

#endif // APP_TASK_MAIN_H
