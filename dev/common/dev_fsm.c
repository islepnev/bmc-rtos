/*
**    Copyright 2021 Ilia Slepnev
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

#include "dev_fsm.h"

#include "cmsis_os.h"
#include "devicebase.h"

void dev_fsm_change(dev_fsm_t *fsm, const fsm_state_t state)
{
    if (state == fsm->state)
        return;
    fsm->stateStartTick = osKernelSysTick();
    fsm->state = state;
}

uint32_t dev_fsm_stateTicks(const dev_fsm_t *fsm)
{
    return osKernelSysTick() - fsm->stateStartTick;
}
