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

#ifndef DEV_AD9548_FSM_H
#define DEV_AD9548_FSM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AD9548_STATE_INIT,
    AD9548_STATE_RESET,
    AD9548_STATE_SETUP_SYSCLK,
    AD9548_STATE_SYSCLK_WAITLOCK,
    AD9548_STATE_SETUP,
    AD9548_STATE_RUN,
    AD9548_STATE_ERROR,
    AD9548_STATE_FATAL
} ad9548_state_t;

struct Dev_ad9548;
void dev_ad9548_run(struct Dev_ad9548 *dev, bool enable);

#ifdef __cplusplus
}
#endif

#endif // DEV_AD9548_FSM_H
