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

#ifndef DEV_AD9545_FSM_H
#define DEV_AD9545_FSM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLL_STATE_INIT,
    PLL_STATE_RESET,
    PLL_STATE_SETUP_SYSCLK,
    PLL_STATE_SYSCLK_WAITLOCK,
    PLL_STATE_SETUP,
    PLL_STATE_RUN,
    PLL_STATE_ERROR,
    PLL_STATE_FATAL
} ad9545_state_t;

void dev_ad9545_run(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_AD9545_FSM_H
